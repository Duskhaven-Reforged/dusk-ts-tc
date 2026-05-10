-- Cleanup for DBErrors group 5: SmartAI structural/link errors.

-- Persist load-time "not repeatable" correction for timed SmartAI events with
-- zero repeat timers.
UPDATE `default.dataset.world.dest`.`smart_scripts`
SET `event_flags` = `event_flags` | 1
WHERE `source_type` <> 9
  AND (`event_flags` & 1) = 0
  AND (
       (`event_type` IN (0,1,2,3,9,12,14,16,18,23,24,60,67)
        AND `event_param3` = 0
        AND `event_param4` = 0)
    OR (`event_type` IN (13,15)
        AND `event_param1` = 0
        AND `event_param2` = 0)
  );

-- Broken link fields should not point at missing events.
UPDATE `default.dataset.world.dest`.`smart_scripts` ss
LEFT JOIN `default.dataset.world.dest`.`smart_scripts` linked
  ON linked.`entryorguid` = ss.`entryorguid`
 AND linked.`source_type` = ss.`source_type`
 AND linked.`id` = ss.`link`
SET ss.`link` = 0
WHERE ss.`link` <> 0
  AND linked.`entryorguid` IS NULL;

-- Link events without a source event can never trigger.
DELETE link_event
FROM `default.dataset.world.dest`.`smart_scripts` link_event
LEFT JOIN `default.dataset.world.dest`.`smart_scripts` source_event
  ON source_event.`entryorguid` = link_event.`entryorguid`
 AND source_event.`source_type` = link_event.`source_type`
 AND source_event.`link` = link_event.`id`
WHERE link_event.`event_type` = 61
  AND source_event.`entryorguid` IS NULL;

-- Creature GUID SmartAI rows are valid only if the spawned template uses SmartAI.
DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN `default.dataset.world.dest`.`creature` cr ON cr.`guid` = -ss.`entryorguid`
JOIN `default.dataset.world.dest`.`creature_template` ct ON ct.`entry` = cr.`id`
WHERE ss.`source_type` = 0
  AND ss.`entryorguid` < 0
  AND ct.`AIName` <> 'SmartAI';
