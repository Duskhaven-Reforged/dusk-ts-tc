#include "Area.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "ZoneScript.h"

AreaMgr* AreaMgr::instance()
{
    static AreaMgr instance;
    return &instance;
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
    m_zoneScript = sScriptMgr->GetZoneScript(sObjectMgr->GetScriptIdForZone(GetId()));

    // Calculate parent & zone at creation
    m_parent = sAreaMgr->GetArea(GetEntry()->ParentAreaID);

    m_zone = m_parent ? m_parent: this;
    while (m_zone && !m_zone->IsZone())
        m_zone = sAreaMgr->GetArea(m_zone->GetEntry()->ParentAreaID);
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

ZoneScript* Area::GetZoneScript()
{
    if (m_zoneScript)
        return m_zoneScript;

    if (Area* parent = GetParent())
        return parent->GetZoneScript();

    return nullptr;
}
