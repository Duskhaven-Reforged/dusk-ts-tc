-- Cleanup for DBErrors group 9: item references and item_template load fixes.
-- Exact IDs from DBErrors.log only.

DROP TEMPORARY TABLE IF EXISTS tmp_bad_item_entries;
CREATE TEMPORARY TABLE tmp_bad_item_entries (
  `entry` INT UNSIGNED NOT NULL PRIMARY KEY
) ENGINE=Memory;

INSERT IGNORE INTO tmp_bad_item_entries (`entry`) VALUES
(60211),(60377),(61047),(61048),(61049),(61050),(61051),(61052),(61053),(61054),
(61055),(61056),(61057),(61058),(61061),(61062),(61063),(61070),(61106),(61107),
(61125),(61126),(61127),(61128),(61129);

DELETE it
FROM `default.dataset.world.dest`.`item_template` it
JOIN tmp_bad_item_entries bad ON bad.`entry` = it.`entry`;

DELETE isn
FROM `default.dataset.world.dest`.`item_set_names` isn
JOIN tmp_bad_item_entries bad ON bad.`entry` = isn.`entry`;

DELETE nv
FROM `default.dataset.world.dest`.`npc_vendor` nv
JOIN tmp_bad_item_entries bad ON bad.`entry` = nv.`item`;

DELETE genv
FROM `default.dataset.world.dest`.`game_event_npc_vendor` genv
JOIN tmp_bad_item_entries bad ON bad.`entry` = genv.`item`;

-- Clear exact bad quest item refs rather than deleting quests.
UPDATE `default.dataset.world.dest`.`quest_template`
SET `RequiredItemId1` = 0, `RequiredItemCount1` = 0
WHERE (`ID`,`RequiredItemId1`) IN ((7962,19482),(8530,20737),(8617,20737),(9308,22989),(9507,23740),(9695,24228));

UPDATE `default.dataset.world.dest`.`quest_template`
SET `RewardItem1` = 0, `RewardAmount1` = 0
WHERE (`ID`,`RewardItem1`) IN ((9411,23689),(9414,23689),(9458,23696),(9459,23696),(9477,23698),(9478,23699),(9479,23700),(9480,23701),(9481,23699),(9482,23698));

-- Invalid spell learn refs in item_template.
UPDATE `default.dataset.world.dest`.`item_template`
SET `spellid_1` = 0, `spellid_2` = 0, `spelltrigger_2` = 0
WHERE `spellid_1` IN (483,55884)
  AND (`entry`,`spellid_2`) IN ((3830,3450),(10605,12615),(12682,16642),(12683,16643),(12684,16644),(12693,16652),(12694,16653),(12704,16662),(12819,16969),(12823,16971),(13476,17552),(13477,17553),(13479,17555),(13480,17556),(13481,17557),(13492,17572),(13493,17573),(14466,18403),(14469,18406),(14470,18407),(14472,18409),(14473,18410),(14474,18411),(14477,18413),(14478,18414),(14479,18415),(14480,18416),(14481,18417),(14484,18420),(14485,18421),(14488,18423),(14489,18424),(14491,18438),(14492,18437),(14495,18441),(14496,18442),(14498,18444),(14500,18446),(14504,18449),(14505,18450),(14506,18451),(14508,18453),(15728,19052),(15731,19055),(15739,19065),(15744,19071),(15745,19072),(15756,19082),(15757,19083),(15765,19091),(15768,19092),(15773,19098),(15776,19102),(15777,19103),(16041,19790),(16042,19791),(16043,19792),(16045,19794),(16047,19795),(16051,19800),(16053,19825),(18651,23071),(21279,25306),(21281,25307),(22146,26991),(22153,27127),(23130,28903),(23135,28910),(23140,28916),(23144,28925),(23148,28938),(23152,28950),(29549,25392),(29550,27090),(31501,33717),(31837,39374),(49177,54197));

UPDATE `default.dataset.world.dest`.`item_template`
SET `spellid_2` = 0
WHERE (`entry`,`spellid_2`) IN ((3830,3450),(10605,12615),(12682,16642),(12683,16643),(12684,16644),(12693,16652),(12694,16653),(12704,16662),(12819,16969),(12823,16971),(13476,17552),(13477,17553),(13479,17555),(13480,17556),(13481,17557),(13492,17572),(13493,17573),(14466,18403),(14469,18406),(14470,18407),(14472,18409),(14473,18410),(14474,18411),(14477,18413),(14478,18414),(14479,18415),(14480,18416),(14481,18417),(14484,18420),(14485,18421),(14488,18423),(14489,18424),(14491,18438),(14492,18437),(14495,18441),(14496,18442),(14498,18444),(14500,18446),(14504,18449),(14505,18450),(14506,18451),(14508,18453),(15728,19052),(15731,19055),(15739,19065),(15744,19071),(15745,19072),(15756,19082),(15757,19083),(15765,19091),(15768,19092),(15773,19098),(15776,19102),(15777,19103),(16041,19790),(16042,19791),(16043,19792),(16045,19794),(16047,19795),(16051,19800),(16053,19825),(18651,23071),(21279,25306),(21281,25307),(22146,26991),(22153,27127),(23130,28903),(23135,28910),(23140,28916),(23144,28925),(23148,28938),(23152,28950),(29549,25392),(29550,27090),(31501,33717),(31837,39374),(49177,54197));

UPDATE `default.dataset.world.dest`.`item_template`
SET `requiredspell` = 0
WHERE (`entry`,`requiredspell`) IN ((34061,34091),(44558,34091),(44602,27126),(44714,27127),(50435,34091),(54797,34091));

UPDATE `default.dataset.world.dest`.`item_template`
SET `RequiredReputationFaction` = 0, `RequiredReputationRank` = 0
WHERE (`entry`,`RequiredReputationFaction`) IN ((61047,1165),(61048,1165),(61049,1165),(61050,1165),(61051,1165),(61052,1165),(61053,1165),(61054,1165),(61055,1165),(61056,1165),(61058,1165));

UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type2` = 0 WHERE `entry` = 22230 AND `stat_type2` = 50;
UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type3` = 0 WHERE (`entry`,`stat_type3`) IN ((20142,50),(20522,50),(20524,50),(21614,55));
UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type4` = 0 WHERE (`entry`,`stat_type4`) IN ((20142,51),(20524,54));
UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type5` = 0 WHERE (`entry`,`stat_type5`) IN ((20142,52),(23363,55));
UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type6` = 0 WHERE (`entry`,`stat_type6`) IN ((20142,54),(23363,56));
UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type7` = 0 WHERE `entry` = 20142 AND `stat_type7` = 55;
UPDATE `default.dataset.world.dest`.`item_template` SET `stat_type8` = 0 WHERE `entry` = 20142 AND `stat_type8` = 56;

UPDATE `default.dataset.world.dest`.`item_template` SET `sheath` = 2 WHERE `entry` = 60616;
UPDATE `default.dataset.world.dest`.`item_template` SET `sheath` = 0 WHERE `entry` IN (60618,60628,61147);

DELETE FROM `default.dataset.world.dest`.`item_set_names`
WHERE `entry` IN (61150,61151,61165,61166,61167,61168,61169,61170,61171,61174);

DELETE FROM `default.dataset.world.dest`.`npc_vendor`
WHERE (`entry`,`item`,`ExtendedCost`) IN ((47186,4597,4),(47186,13322,400),(47186,14730,4),(47186,22206,4),(47186,45981,4),(47186,60628,4));

DELETE FROM `default.dataset.world.dest`.`game_event_npc_vendor`
WHERE (`guid`,`item`,`ExtendedCost`) IN ((47186,4597,4),(47186,13322,400),(47186,14730,4),(47186,22206,4),(47186,45981,4),(47186,60628,4));

DROP TEMPORARY TABLE IF EXISTS tmp_bad_item_entries;
