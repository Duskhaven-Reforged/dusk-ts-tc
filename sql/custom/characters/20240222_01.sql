-- acore_characters.character_action definition

DROP TABLE IF EXISTS `character_action`;
CREATE TABLE `character_action` (
  `guid` int unsigned NOT NULL DEFAULT '0',
  `spec` tinyint unsigned NOT NULL DEFAULT '0',
  `loadout` int unsigned NOT NULL DEFAULT '1',
  `button` tinyint unsigned NOT NULL DEFAULT '0',
  `action` int unsigned NOT NULL DEFAULT '0',
  `type` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spec`,`loadout`,`button`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


-- acore_characters.character_node_choices definition
DROP TABLE IF EXISTS `character_node_choices`;
CREATE TABLE `character_node_choices` (
  `guid` int unsigned NOT NULL,
  `spec` int unsigned NOT NULL,
  `tabId` int unsigned NOT NULL,
  `node` int unsigned NOT NULL,
  `choice` int unsigned NOT NULL,
  PRIMARY KEY (`guid`,`spec`,`tabId`,`node`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


-- acore_characters.character_points definition
DROP TABLE IF EXISTS `characterpoints`;
CREATE TABLE `characterpoints` (
  `guid` bigint unsigned NOT NULL,
  `type` int unsigned NOT NULL,
  `sum` int unsigned NOT NULL,
  `unlocked` int unsigned NOT NULL,
  `max` int unsigned NOT NULL,
  PRIMARY KEY (`guid`,`type`,`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

DROP TABLE IF EXISTS `characterspellcharges`;
CREATE TABLE `characterspellcharges` (
  `guid` bigint unsigned NOT NULL,
  `spell` bigint unsigned NOT NULL,
  `current` int unsigned NOT NULL,
  `max` int unsigned NOT NULL,
  `cd` bigint unsigned NOT NULL,
  PRIMARY KEY (`guid`,`type`,`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        
-- acore_characters.character_specs definition
DROP TABLE IF EXISTS `character_specs`;
CREATE TABLE `character_specs` (
  `id` int unsigned NOT NULL,
  `guid` int unsigned NOT NULL,
  `name` varchar(45) COLLATE utf8mb4_general_ci NOT NULL,
  `description` varchar(500) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `active` tinyint NOT NULL,
  `spellicon` mediumint unsigned NOT NULL,
  `visability` tinyint unsigned NOT NULL,
  `charSpec` int NOT NULL,
  `archetypalRole` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


-- acore_characters.character_talent_loadouts definition
DROP TABLE IF EXISTS `character_talent_loadouts`;
CREATE TABLE `character_talent_loadouts` (
  `guid` int unsigned NOT NULL,
  `id` int unsigned NOT NULL,
  `talentTabId` int unsigned NOT NULL,
  `name` varchar(45) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `talentString` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `active` tinyint NOT NULL,
  PRIMARY KEY (`guid`,`talentTabId`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


-- acore_characters.character_talents definition
DROP TABLE IF EXISTS `character_talents`;
CREATE TABLE `character_talents` (
  `guid` int unsigned NOT NULL,
  `spec` int unsigned NOT NULL,
  `spellid` mediumint unsigned NOT NULL,
  `tabId` int unsigned NOT NULL,
  `currentrank` tinyint unsigned NOT NULL,
  PRIMARY KEY (`guid`,`spec`,`spellid`,`tabId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


-- acore_characters.character_talents_spent definition
DROP TABLE IF EXISTS `character_talents_spent`;
CREATE TABLE `character_talents_spent` (
  `guid` int unsigned NOT NULL,
  `spec` int unsigned NOT NULL,
  `tabId` int unsigned NOT NULL,
  `spent` tinyint unsigned NOT NULL,
  PRIMARY KEY (`guid`,`spec`,`tabId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT INTO character_action (guid, spec, loadout, button, `action`, `type`) VALUES(0, 0, 1, 0, 0, 0);

INSERT INTO character_node_choices (guid, spec, tabId, node, choice) VALUES(0, 0, 0, 0, 0);

INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(0, 0, 0, 0, 0);
INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 8, 4294967295, 0, 21);
INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 7, 4294967295, 0, 25);
INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 6, 4294967295, 0, 0);
INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 4, 4294967295, 17, 17);
INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 3, 4294967295, 0, 20);
INSERT INTO character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 0, 4294967295, 0, 71);