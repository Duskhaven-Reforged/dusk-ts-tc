-- Cleanup for DBErrors group 4: creature flag auto-corrections.
-- Mirrors TrinityCore load-time masks so persisted .dest data matches runtime.

UPDATE `creature_template`
SET `unit_flags` = `unit_flags` & 33608512
WHERE `unit_flags` <> (`unit_flags` & 33608512);

UPDATE `creature_template`
SET `unit_flags2` = `unit_flags2` & 231458
WHERE `unit_flags2` <> (`unit_flags2` & 231458);

UPDATE `creature_template`
SET `flags_extra` = `flags_extra` & 1614807039
WHERE `flags_extra` <> (`flags_extra` & 1614807039);

UPDATE `creature_template`
SET `dynamicflags` = 0
WHERE `dynamicflags` <> 0;

UPDATE `creature`
SET `unit_flags` = `unit_flags` & 33608512
WHERE `unit_flags` <> (`unit_flags` & 33608512);

UPDATE `creature`
SET `dynamicflags` = 0
WHERE `dynamicflags` <> 0;

-- Idle creatures should not carry wander distance, and random movers need one.
UPDATE `creature`
SET `wander_distance` = 0
WHERE `MovementType` = 0
  AND `wander_distance` <> 0;

UPDATE `creature`
SET `MovementType` = 0
WHERE `MovementType` = 1
  AND `wander_distance` = 0;
