/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "MessageBuffer.h"
#include "Log.h"
#include <atomic>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;

#define READ_BLOCK_SIZE 4096
#ifdef BOOST_ASIO_HAS_IOCP
#define TC_SOCKET_USE_IOCP
#endif

enum class ProxyConnectionState
{
    Idle,
    Started,
    Finished,
    Failed
};

enum class ProxyProtocolFamily
{
    TCP_V4 = 0x11,
    TCP_V6 = 0x21
};

template<class T>
class Socket : public std::enable_shared_from_this<T>
{
public:
    explicit Socket(tcp::socket&& socket) : _socket(std::move(socket)), _readBuffer(), _closed(false), _closing(false), _isWritingAsync(false)
        , _proxyState(ProxyConnectionState::Idle) // Default state, only valid if the network thread is behind a proxy
    {
        _readBuffer.Resize(READ_BLOCK_SIZE);
    }

    /// Do not call this when a server is behind a proxy, the remote_* members will throw an exception
    /// the IP and port are extracted from the proxy header, see ProcessProxyProtocol
    void Initialize()
    {
        _remoteAddress = _socket.remote_endpoint().address();
        _remotePort = _socket.remote_endpoint().port();
    }

    virtual ~Socket()
    {
        _closed = true;
        boost::system::error_code error;
        _socket.close(error);
    }

    virtual void Start() = 0;

    virtual bool Update()
    {
        if (_closed)
            return false;

#ifndef TC_SOCKET_USE_IOCP
        if (_isWritingAsync || (_writeQueue.empty() && !_closing))
            return true;

        for (; HandleQueue();)
            ;
#endif

        return true;
    }

    boost::asio::ip::address GetRemoteIpAddress() const
    {
        return _remoteAddress;
    }

    uint16 GetRemotePort() const
    {
        return _remotePort;
    }

    ProxyConnectionState GetProxyReadState() const
    {
        return _proxyState;
    }

    void AsyncRead()
    {
        if (!IsOpen())
            return;

        _readBuffer.Normalize();
        _readBuffer.EnsureFreeSpace();
        _socket.async_read_some(boost::asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
            std::bind(&Socket<T>::ReadHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void AsyncReadWithCallback(void (T::*callback)(boost::system::error_code, std::size_t))
    {
        if (!IsOpen())
            return;

        _readBuffer.Normalize();
        _readBuffer.EnsureFreeSpace();
        _socket.async_read_some(boost::asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
            std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void AsyncReadProxyHeader()
    {
        if (!IsOpen())
            return;

        _proxyState = ProxyConnectionState::Started;
        _readBuffer.Normalize();
        _readBuffer.EnsureFreeSpace();
        _socket.async_read_some(boost::asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
            std::bind(&Socket<T>::ProcessProxyProtocol, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void QueuePacket(MessageBuffer&& buffer)
    {
        _writeQueue.push(std::move(buffer));

#ifdef TC_SOCKET_USE_IOCP
        AsyncProcessQueue();
#endif
    }

    bool IsOpen() const { return !_closed && !_closing; }

    void CloseSocket()
    {
        if (_closed.exchange(true))
            return;

        boost::system::error_code shutdownError;
        _socket.shutdown(boost::asio::socket_base::shutdown_send, shutdownError);
        if (shutdownError)
            TC_LOG_DEBUG("network", "Socket::CloseSocket: {} errored when shutting down socket: {} ({})", GetRemoteIpAddress().to_string(),
                shutdownError.value(), shutdownError.message());

        OnClose();
    }

    /// Marks the socket for closing after write buffer becomes empty
    void DelayedCloseSocket()
    {
        if (_closing.exchange(true))
            return;

        if (_writeQueue.empty())
            CloseSocket();
    }

    MessageBuffer& GetReadBuffer() { return _readBuffer; }

protected:
    virtual void OnClose() { }

    virtual void ReadHandler() = 0;

    bool AsyncProcessQueue()
    {
        if (_isWritingAsync)
            return false;

        _isWritingAsync = true;

#ifdef TC_SOCKET_USE_IOCP
        MessageBuffer& buffer = _writeQueue.front();
        _socket.async_write_some(boost::asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()), std::bind(&Socket<T>::WriteHandler,
            this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#else
        _socket.async_write_some(boost::asio::null_buffers(), std::bind(&Socket<T>::WriteHandlerWrapper,
            this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#endif

        return false;
    }

    void SetNoDelay(bool enable)
    {
        boost::system::error_code err;
        _socket.set_option(tcp::no_delay(enable), err);
        if (err)
            TC_LOG_DEBUG("network", "Socket::SetNoDelay: failed to set_option(boost::asio::ip::tcp::no_delay) for {} - {} ({})",
                GetRemoteIpAddress().to_string(), err.value(), err.message());
    }

private:
    // See https://www.haproxy.org/download/1.8/doc/proxy-protocol.txt (2.2. Binary header format (version 2)) for more details.
    void ProcessProxyProtocol(boost::system::error_code error, std::size_t transferredBytes)
    {
        if (error)
        {
            CloseSocket();
            return;
        }

        _readBuffer.WriteCompleted(transferredBytes);

        MessageBuffer& packet = GetReadBuffer();

        const int minimumProxyProtocolV2Size = 28;
        if (packet.GetActiveSize() < minimumProxyProtocolV2Size)
        {
            AsyncReadProxyHeader();
            return;
        }

        uint8* readPointer = packet.GetReadPointer();

        const uint8 signatureSize = 12;
        const uint8 expectedSignature[signatureSize] = { 0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A };
        if (memcmp(packet.GetReadPointer(), expectedSignature, signatureSize) != 0)
        {
            _proxyState = ProxyConnectionState::Failed;
            // we cannot deduce ip, ip is stored in the proxy header pkt which we failed to deserialize
            TC_LOG_ERROR("network", "ReadProxyHeader: Some ip sent bad PROXY Protocol v2 signature");
            return;
        }

        const uint8 version = (readPointer[signatureSize] & 0xF0) >> 4;
        const uint8 command = (readPointer[signatureSize] & 0xF);
        if (version != 2)
        {
            _proxyState = ProxyConnectionState::Failed;
            TC_LOG_ERROR("network", "ReadProxyHeader: Some ip sent bad PROXY Protocol v2 version");
            return;
        }

        const uint8 addressFamily = readPointer[13];
        const uint16 len = (readPointer[14] << 8) | readPointer[15];
        if (len + 16 > packet.GetActiveSize())
        {
            AsyncReadProxyHeader();
            return;
        }

        // Connection created by a proxy itself (health checks?), ignore and do nothing.
        if (command == 0)
        {
            packet.ReadCompleted(len + 16);
            _proxyState = ProxyConnectionState::Finished;
            TC_LOG_ERROR("network", "ReadProxyHeader: Some ip sent PROXY Protocol v2 command 0");
            // TODO actually check what to dowith command 0, we just let them go without assigning ip?
            return;
        }

        auto remainingLen = packet.GetActiveSize() - 16;
        readPointer += 16; // Actual data begins here

        switch (static_cast<ProxyProtocolFamily>(addressFamily))
        {
            case ProxyProtocolFamily::TCP_V4:
            {
                if (remainingLen < 12)
                {
                    AsyncReadProxyHeader();
                    return;
                }

                boost::asio::ip::address_v4::bytes_type b;
                auto addressSize = sizeof(b);
                std::copy(readPointer, readPointer + addressSize, b.begin());
                _remoteAddress = boost::asio::ip::address_v4(b);
                readPointer += 2 * addressSize; // Skip server address.
                _remotePort = (readPointer[0] << 8) | readPointer[1];
                break;
            }
            case ProxyProtocolFamily::TCP_V6:
            {
                if (remainingLen < 36)
                {
                    AsyncReadProxyHeader();
                    return;
                }

                boost::asio::ip::address_v6::bytes_type b;
                auto addressSize = sizeof(b);
                std::copy(readPointer, readPointer + addressSize, b.begin());
                _remoteAddress = boost::asio::ip::address_v6(b);
                readPointer += 2 * addressSize; // Skip server address.
                _remotePort = (readPointer[0] << 8) | readPointer[1];
                break;
            }
            default:
                _proxyState = ProxyConnectionState::Failed;
                TC_LOG_ERROR("network", "ReadProxyHeader: Some ip sent unsupported PROXY Protocol v2 address family type");
                return; // Exit out of function, do not go further
        }

        packet.ReadCompleted(len + 16);
        _proxyState = ProxyConnectionState::Finished;
    }

    void ReadHandlerInternal(boost::system::error_code error, size_t transferredBytes)
    {
        if (error)
        {
            CloseSocket();
            return;
        }

        _readBuffer.WriteCompleted(transferredBytes);
        ReadHandler();
    }

#ifdef TC_SOCKET_USE_IOCP

    void WriteHandler(boost::system::error_code error, std::size_t transferedBytes)
    {
        if (!error)
        {
            _isWritingAsync = false;
            _writeQueue.front().ReadCompleted(transferedBytes);
            if (!_writeQueue.front().GetActiveSize())
                _writeQueue.pop();

            if (!_writeQueue.empty())
                AsyncProcessQueue();
            else if (_closing)
                CloseSocket();
        }
        else
            CloseSocket();
    }

#else

    void WriteHandlerWrapper(boost::system::error_code /*error*/, std::size_t /*transferedBytes*/)
    {
        _isWritingAsync = false;
        HandleQueue();
    }

    bool HandleQueue()
    {
        if (_writeQueue.empty())
            return false;

        MessageBuffer& queuedMessage = _writeQueue.front();

        std::size_t bytesToSend = queuedMessage.GetActiveSize();

        boost::system::error_code error;
        std::size_t bytesSent = _socket.write_some(boost::asio::buffer(queuedMessage.GetReadPointer(), bytesToSend), error);

        if (error)
        {
            if (error == boost::asio::error::would_block || error == boost::asio::error::try_again)
                return AsyncProcessQueue();

            _writeQueue.pop();
            if (_closing && _writeQueue.empty())
                CloseSocket();
            return false;
        }
        else if (bytesSent == 0)
        {
            _writeQueue.pop();
            if (_closing && _writeQueue.empty())
                CloseSocket();
            return false;
        }
        else if (bytesSent < bytesToSend) // now n > 0
        {
            queuedMessage.ReadCompleted(bytesSent);
            return AsyncProcessQueue();
        }

        _writeQueue.pop();
        if (_closing && _writeQueue.empty())
            CloseSocket();
        return !_writeQueue.empty();
    }

#endif

    tcp::socket _socket;

    boost::asio::ip::address _remoteAddress;
    uint16 _remotePort;

    MessageBuffer _readBuffer;
    std::queue<MessageBuffer> _writeQueue;

    std::atomic<bool> _closed;
    std::atomic<bool> _closing;

    bool _isWritingAsync;
    ProxyConnectionState _proxyState;
};

#endif // __SOCKET_H__