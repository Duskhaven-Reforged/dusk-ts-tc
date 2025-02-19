#include "Area.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#include "AreaScript.h"

AreaMgr* AreaMgr::instance()
{
    static AreaMgr instance;
    return &instance;
}

void AreaMgr::Update(uint32 diff) {
    updateTicker += diff;
    if (updateTicker >= AREAUPDATE_INTERVAL) {
        for (const auto& kvp : m_areas) {
            Area* area = kvp.second;
            if (AreaScript* script = area->GetAreaScript())
                area->GetAreaScript()->Update(diff);
        }
        updateTicker = 0;
    }
}

void AreaMgr::HandlePlayerLeaveZone(Area* zone, Player* player) {
    if (auto as = zone->GetAreaScript())
        as->OnPlayerLeave(player);
}

void AreaMgr::HandlePlayerEnterZone(Area* zone, Player* player)
{
    if (auto as = zone->GetAreaScript())
        as->OnPlayerEnter(player);
}

Area* AreaMgr::GetArea(uint32 areaId)
{
    if (!areaId)
        return nullptr;

    auto itr = m_areas.find(areaId);
    if (itr != m_areas.end())
        return itr->second;

    if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(areaId))
    {
        Area* area = new Area(areaEntry);
        m_areas[areaEntry->ID] = area;
        return area;
    }

    return nullptr;
}

Area::Area(AreaTableEntry const* areaTableEntry) : m_areaTableEntry(areaTableEntry)
{
    auto Continent = areaTableEntry->ContinentID;
    m_areaScript = new AreaScript(this);
    // Calculate parent & zone at creation
    m_parent = sAreaMgr->GetArea(GetEntry()->ParentAreaID);
    m_zone   = this;
    m_map    = sMapMgr->FindBaseNonInstanceMap(Continent);
}

std::vector<Area*> Area::GetTree()
{
    Area* area = this;
    std::vector<Area*> areas;
    while (area)
    {
        areas.push_back(area);
        area = sAreaMgr->GetArea(area->GetEntry()->ParentAreaID);
    }

    return areas;
}

AreaScript* Area::GetAreaScript()
{
    if (m_areaScript)
        return m_areaScript;

    if (Area* parent = GetParent())
        return parent->GetAreaScript();

    return nullptr;
}

std::vector<Player*> Area::GetPlayers()
{
    return GetAreaScript()->GetPlayers();
}

std::vector<Creature*> Area::GetCreatures()
{
    return GetAreaScript()->GetCreatures();
}
