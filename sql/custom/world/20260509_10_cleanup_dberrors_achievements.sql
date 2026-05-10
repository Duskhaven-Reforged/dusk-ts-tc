-- Cleanup for DBErrors group 8: achievement/faction-change references.
-- Exact IDs from DBErrors.log only.

DELETE FROM `default.dataset.world.dest`.`achievement_reward`
WHERE `ID` IN (13,2051,2054,2186,2187,2760,2761,2762,2763,2764,2765,2766,2767,2768,2769,2816,2817,2903,2904,3036,3037,3316,4080,4530,4583,4584,4597,4598);

DELETE arl
FROM `default.dataset.world.dest`.`achievement_reward_locale` arl
LEFT JOIN `default.dataset.world.dest`.`achievement_reward` ar ON ar.`ID` = arl.`ID`
WHERE ar.`ID` IS NULL
  AND arl.`ID` IN (13,2051,2054,2186,2187,2760,2761,2762,2763,2764,2765,2766,2767,2768,2769,2816,2817,2903,2904,3036,3037,3316,4080,4530,4583,4584,4597,4598);

UPDATE `default.dataset.world.dest`.`achievement_reward`
SET `Subject` = '', `Body` = '', `MailTemplateID` = 0
WHERE `ID` = 4903
  AND `Sender` = 0;

DELETE FROM `default.dataset.world.dest`.`player_factionchange_achievement`
WHERE `alliance_id` IN (33,34,35,37,41,764,899,1012,1189,1191,1192,1262,2760,2761,2762,2763,2764,2770,2777,2778,2779,2780,2781,2782,2817,3676,4296,4298)
   OR `horde_id` IN (33,34,35,37,41,764,899,1012,1189,1191,1192,1262,2760,2761,2762,2763,2764,2770,2777,2778,2779,2780,2781,2782,2817,3676,4296,4298);

DELETE FROM `default.dataset.world.dest`.`conditions`
WHERE `ConditionTypeOrReference` = 17
  AND `ConditionValue1` IN (41,1360,2777,2778,2779,2780,2781,2783,2784,2785,2786,2787,2816,2817,3736,3916,3917,4530,4597);
