-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Conditions referencing removed creatures/invalid instance data.
DELETE FROM `conditions`
WHERE (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ConditionTypeOrReference, ConditionValue1) IN (
    (15, 58038, 0, 0, 9, 45077),
    (13, 1, 27892, 0, 31, 127618),
    (13, 1, 27892, 0, 31, 127619),
    (13, 1, 27893, 0, 31, 127620),
    (13, 1, 27893, 0, 31, 127621),
    (13, 1, 27928, 0, 31, 127618),
    (13, 1, 27928, 0, 31, 127619),
    (13, 1, 27929, 0, 31, 127620),
    (13, 1, 27929, 0, 31, 127621),
    (13, 1, 27935, 0, 31, 127618),
    (13, 1, 27935, 0, 31, 127619),
    (13, 1, 27936, 0, 31, 127621),
    (13, 1, 27936, 0, 31, 127620),
    (13, 1, 39635, 0, 31, 52502),
    (13, 1, 39849, 0, 31, 52503),
    (22, 5, 1913, 0, 38, 70),
    (22, 4, 47589, 0, 38, 70)
);
