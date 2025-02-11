#ifndef _AREA_H
#define _AREA_H

 // @tswow-begin
#include "TSEntity.h"
// @tswow-end
#include "DBCStructure.h"
#include "Position.h"
#include "ZoneScript.h"

class Area;
class ZoneScript;

struct GameObjectTemplate;

class TC_GAME_API AreaMgr
{
public:
    static AreaMgr* instance();

    Area* GetArea(uint32 areaId);

private:
    std::map<uint32, Area*> m_areas;
};

#define sAreaMgr AreaMgr::instance()

class TC_GAME_API Area
{

public:
    Area(AreaTableEntry const* areaTableEntry);

    AreaTableEntry const* GetEntry() const { return m_areaTableEntry; }
    uint32 GetId() const { return GetEntry()->ID; }

    bool IsArea() const { return m_areaTableEntry->Flags & AREA_FLAG_SUB_ZONE; }
    bool IsZone() const { return !IsArea(); }

    Area* GetParent() { return m_parent; }
    Area* GetZone() { return m_zone; }
    std::vector<Area*> GetTree();

    ZoneScript* GetZoneScript();

private:
    AreaTableEntry const* m_areaTableEntry;
    ZoneScript* m_zoneScript;

    std::unordered_map<uint8, std::unordered_map<size_t, std::vector<ObjectGuid::LowType>>> m_gatheringNodes;

    Area* m_parent;
    Area* m_zone;
};

#endif
