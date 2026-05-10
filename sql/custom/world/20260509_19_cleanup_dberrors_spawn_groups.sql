-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Default group id must exist before rows can reference it.
-- Group 0 is TrinityCore's implicit default spawn group and must be system-wide.
INSERT INTO `default.dataset.world.dest`.`spawn_group_template` (groupId, groupName, groupFlags)
VALUES (0, 'Default Group', 1)
ON DUPLICATE KEY UPDATE groupName = VALUES(groupName), groupFlags = VALUES(groupFlags);

-- Remove spawn group rows whose spawn map does not match the group map constraint.
DELETE FROM `default.dataset.world.dest`.`spawn_group`
WHERE (groupId, spawnType, spawnId) IN (
    (61, 1, 94839),
    (61, 1, 94840)
);
