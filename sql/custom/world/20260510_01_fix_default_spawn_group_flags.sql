-- Manual cleanup for TrinityCore default spawn group metadata.
-- Group 0 is queried as the implicit default group by normal spawns; it must be system-wide.

INSERT INTO `spawn_group_template` (groupId, groupName, groupFlags)
VALUES (0, 'Default Group', 1)
ON DUPLICATE KEY UPDATE groupName = VALUES(groupName), groupFlags = VALUES(groupFlags);
