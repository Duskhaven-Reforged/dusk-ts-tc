#ifndef _AREASCRIPT_H
#define _AREASCRIPT_H

#include "ZoneScript.h"
#include "Common.h"
#include "Map.h"
#include "ObjectAccessor.h"
#endif

class Area;

class TC_GAME_API AreaScript : public ZoneScript
{
  public:
    AreaScript(Area* area);
    ~AreaScript();

    virtual void Update(uint32 diff);

    virtual void OnCreatureCreate(Creature* creature) override;
    virtual void OnCreatureRemove(Creature* creature) override;
    virtual void OnCreatureRespawn(Creature*) override;

    virtual void OnGameObjectCreate(GameObject* go) override;
    virtual void OnGameObjectRemove(GameObject* go) override;
    virtual void OnGameObjectUsed(GameObject* go, Unit* by) override;

    void OnUnitKilled(Unit*, Unit*) override;

    // Called when a player successfully enters the instance.
    virtual void OnPlayerEnter(Player* /*player*/);
    // Called when a player successfully leaves the instance.
    virtual void OnPlayerLeave(Player* /*player*/);

    uint32 GetPlayerCount() {
        return m_activePlayers.size();
    }

    std::vector<Player*> GetPlayers() {
        std::vector<Player*> out = {};
        for (auto p : m_activePlayers)
            out.push_back(ObjectAccessor::FindPlayer(p));

        return out;
    }

    std::vector<Creature*> GetCreatures() {
        std::vector<Creature*> out = {};
        for (auto p : m_creatures)
            if (Map* map = area->GetMap())
                out.push_back(map->GetCreature(p));

        return out;
    }

  private:
    GuidSet m_activePlayers = {};
    GuidSet m_creatures = {};

    Area* area;
};
