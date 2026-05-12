#include "WorldBotSession.h"
#include "WorldSession.h"
#include <utility>

std::unique_ptr<WorldSession> WorldBotSession::Create(uint32 accountId, std::string accountName, uint8 expansion, LocaleConstant locale)
{
    std::shared_ptr<WorldSocket> socket;
    return std::make_unique<WorldSession>(accountId, std::move(accountName), std::move(socket), SEC_PLAYER, expansion, 0,
        Minutes(0), locale, 0, false, WorldSessionKind::WorldBot);
}
