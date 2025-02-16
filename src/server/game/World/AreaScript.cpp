#include "AreaScript.h"
#include "Creature.h"
#include "Player.h"
#include "GameObject.h"
#include "ObjectMgr.h"

#include "TSArea.h"
#include "TSCreature.h"
#include "TSPlayer.h"
#include "TSGameObject.h"

AreaScript::AreaScript(Area* area) : area(area)
{
	m_activePlayers.clear();
}

AreaScript::~AreaScript()
{
    m_activePlayers.clear();
}

void AreaScript::OnUnitKilled(Unit* who, Unit* by)
{
    if (Player* player = who->ToPlayer())
    {
        FIRE_ID(area->GetId(), Area, OnPlayerDied, TSArea(area), TSPlayer(player), TSUnit(by));
    }
    else if (Creature* creature = who->ToCreature())
        FIRE_ID(area->GetId(), Area, OnCreatureDied, TSArea(area), TSCreature(creature), TSUnit(by));
}

void AreaScript::OnCreatureCreate(Creature* creature)
{
    FIRE_ID(area->GetId(), Area, OnCreatureCreate, TSArea(area), TSCreature(creature));
}

void AreaScript::OnCreatureRemove(Creature* creature)
{
    FIRE_ID(area->GetId(), Area, OnCreatureRemove, TSArea(area), TSCreature(creature));
}

void AreaScript::OnGameObjectCreate(GameObject* go)
{
    FIRE_ID(area->GetId(), Area, OnGameObjectCreate, TSArea(area), TSGameObject(go));
}

void AreaScript::OnGameObjectRemove(GameObject* go)
{
    FIRE_ID(area->GetId(), Area, OnGameObjectRemove, TSArea(area), TSGameObject(go));
}

void AreaScript::OnGameObjectUsed(GameObject* go, Unit* by) {
    FIRE_ID(area->GetId(), Area, OnGameObjectUsed, TSArea(area), TSGameObject(go), TSUnit(by));
}

void AreaScript::Update(uint32 diff)
{
    FIRE_ID(area->GetId(), Area, OnUpdate, TSArea(area), diff);
}

void AreaScript::OnPlayerEnter(Player* player)
{
    FIRE_ID(area->GetId(), Area, OnPlayerEnter, TSArea(area), TSPlayer(player));
    m_activePlayers.insert(player->GetGUID());

    if (Area* parent = area->GetParent())
        if (AreaScript* as = parent->GetAreaScript())
            as->OnPlayerEnter(player);
}

void AreaScript::OnPlayerLeave(Player* player)
{
    FIRE_ID(area->GetId(), Area, OnPlayerLeave, TSArea(area), TSPlayer(player));
    m_activePlayers.erase(player->GetGUID());

    if (Area* parent = area->GetParent())
        if (AreaScript* as = parent->GetAreaScript())
            as->OnPlayerLeave(player);
}

void AreaScript::OnCreatureRespawn(Creature* creature) {
    FIRE_ID(area->GetId(), Area, OnCreatureRespawn, TSArea(area), TSCreature(creature));
}
