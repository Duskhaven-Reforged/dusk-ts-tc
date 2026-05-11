-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Link events without a valid source event never trigger.
UPDATE `smart_scripts`
SET link = 0
WHERE (entryorguid, source_type, id) IN (
    (644, 0, 5),
    (10812, 0, 1),
    (10812, 0, 16),
    (17877, 0, 23),
    (23618, 0, 5),
    (19937, 0, 8),
    (19937, 0, 9),
    (17826, 0, 14),
    (17893, 0, 8),
    (17893, 0, 9),
    (20071, 0, 9),
    (20071, 0, 10)
);
