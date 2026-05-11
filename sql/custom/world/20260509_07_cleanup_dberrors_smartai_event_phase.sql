-- Cleanup for DBErrors group 6: invalid SmartAI action 66 rows.
-- In this core, action 66 (SET_ORIENTATION) collides numerically with the
-- unused phase-change event validator. Rows with action_param1 0 or > 4095 are
-- skipped at load now, so remove them to persist current runtime behavior.

DELETE FROM `smart_scripts`
WHERE `action_type` = 66
  AND (`action_param1` = 0 OR `action_param1` > 4095);

UPDATE `smart_scripts` ss
LEFT JOIN `smart_scripts` linked
  ON linked.`entryorguid` = ss.`entryorguid`
 AND linked.`source_type` = ss.`source_type`
 AND linked.`id` = ss.`link`
SET ss.`link` = 0
WHERE ss.`link` <> 0
  AND linked.`entryorguid` IS NULL;

DELETE link_event
FROM `smart_scripts` link_event
LEFT JOIN `smart_scripts` source_event
  ON source_event.`entryorguid` = link_event.`entryorguid`
 AND source_event.`source_type` = link_event.`source_type`
 AND source_event.`link` = link_event.`id`
WHERE link_event.`event_type` = 61
  AND source_event.`entryorguid` IS NULL;
