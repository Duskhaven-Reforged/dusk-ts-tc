-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Remove SmartAI rows with missing creatures, bad text, bad targets, bad spells, bad summons, or retired quests.
DELETE FROM `default.dataset.world.dest`.`smart_scripts`
WHERE (entryorguid, source_type, id) IN (
    (263, 0, 1),
    (468, 0, 0),
    (468, 0, 1),
    (468, 0, 2),
    (468, 0, 3),
    (10812, 0, 0),
    (10812, 0, 9),
    (10812, 0, 15),
    (643, 0, 1),
    (10812, 0, 18),
    (10812, 0, 20),
    (1888, 0, 6),
    (1912, 0, 4),
    (1913, 0, 2),
    (47399, 0, 3),
    (47589, 0, 1),
    (47590, 0, 1),
    (47591, 0, 1),
    (47673, 0, 2),
    (15938, 0, 9),
    (15938, 0, 10),
    (25087, 0, 1),
    (47164, 0, 1),
    (47414, 0, 0),
    (47414, 0, 2),
    (47414, 0, 3),
    (17826, 0, 13),
    (18471, 0, 26),
    (2724800, 9, 1),
    (2740900, 9, 1),
    (2830800, 9, 1)
);

-- Timed/action events with no repeat window need NOT_REPEATABLE.
UPDATE `default.dataset.world.dest`.`smart_scripts`
SET event_flags = event_flags | 1
WHERE (entryorguid, source_type, id) IN (
    (644, 0, 6),
    (644, 0, 7),
    (10812, 0, 0),
    (11517, 0, 3),
    (11518, 0, 4),
    (11519, 0, 5),
    (11519, 0, 7),
    (45011, 0, 2),
    (47135, 0, 1),
    (47348, 0, 0),
    (47430, 0, 1),
    (47432, 0, 2),
    (47433, 0, 1),
    (47459, 0, 0)
);

-- Link targets that point at missing rows.
UPDATE `default.dataset.world.dest`.`smart_scripts`
SET link = 0
WHERE (entryorguid, source_type, id) IN (
    (644, 0, 5),
    (10812, 0, 1),
    (10812, 0, 16),
    (17877, 0, 23),
    (19455, 0, 4),
    (23618, 0, 5),
    (19937, 0, 8),
    (17826, 0, 14),
    (17893, 0, 8),
    (20071, 0, 9)
);
