#ifndef _AREA_H
#define _AREA_H

 // @tswow-begin
#include "TSEntity.h"
// @tswow-end
#include "DBCStructure.h"
#include "Position.h"

class AreaScript;
class Area;
class Player;
class Creature;
class Map;

struct GameObjectTemplate;

#define AREAUPDATE_INTERVAL 5000

class TC_GAME_API AreaMgr
{
public:
    static AreaMgr* instance();

    Area* GetArea(uint32 areaId);

    void Update(uint32 diff);

    void HandlePlayerLeaveZone(Area* zone, Player* player);
    void HandlePlayerEnterZone(Area* zone, Player* player);

  private:
    std::map<uint32, Area*> m_areas;
    uint32 updateTicker = 0;
};

#define sAreaMgr AreaMgr::instance()

class TC_GAME_API Area
{
    // @tswow-begin
    TSEntity m_tsEntity;
    friend class TSArea;
    // @tswow-end

public:
    Area(AreaTableEntry const* areaTableEntry);

    AreaTableEntry const* GetEntry() const { return m_areaTableEntry; }
    uint32 GetId() const { return GetEntry()->ID; }

    bool IsArea() const { return m_areaTableEntry->Flags & AREA_FLAG_SUB_ZONE; }
    bool IsZone() const { return !IsArea(); }

    Area* GetParent() { return m_parent; }
    Area* GetZone() { return m_zone; }
    std::vector<Area*> GetTree();

    Map* GetMap() { return m_map; }

    AreaScript* GetAreaScript();

    std::vector<Player*> GetPlayers();
    std::vector<Creature*> GetCreatures();

private:
    AreaTableEntry const* m_areaTableEntry;
    AreaScript* m_areaScript;

    Area* m_parent;
    Area* m_zone;

    Map* m_map;
};

#endif
