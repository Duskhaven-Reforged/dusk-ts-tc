#ifndef TRINITY_WORLDBOTSESSION_H
#define TRINITY_WORLDBOTSESSION_H

#include "Common.h"
#include <memory>
#include <string>

class WorldSession;

class TC_GAME_API WorldBotSession
{
public:
    static std::unique_ptr<WorldSession> Create(uint32 accountId, std::string accountName, uint8 expansion, LocaleConstant locale = LOCALE_enUS);
};

#endif
