-- Dumping structure for table acore_world.npc_vendor
CREATE TABLE IF NOT EXISTS `npc_vendor` (
  `entry` int unsigned NOT NULL DEFAULT '0',
  `slot` smallint NOT NULL DEFAULT '0',
  `item` int NOT NULL DEFAULT '0',
  `maxcount` tinyint unsigned NOT NULL DEFAULT '0',
  `incrtime` int unsigned NOT NULL DEFAULT '0',
  `ExtendedCost` int unsigned NOT NULL DEFAULT '0',
  `VerifiedBuild` int DEFAULT NULL,
  PRIMARY KEY (`entry`,`item`,`ExtendedCost`),
  KEY `slot` (`slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Npc System';


REPLACE INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
	(9000153, 0, 10421, 0, 0, 0, NULL),
	(10000396, 0, 20390, 0, 0, 0, 0),
	(10000396, 0, 100103, 0, 0, 0, 0),
	(10000396, 0, 100104, 0, 0, 0, 0),
	(10000396, 0, 100105, 0, 0, 0, 0),
	(10000421, 0, 159, 0, 0, 0, 0),
	(10000421, 0, 1179, 0, 0, 0, 0),
	(10000421, 0, 1205, 0, 0, 0, 0),
	(10000421, 0, 1645, 0, 0, 0, 0),
	(10000421, 0, 1708, 0, 0, 0, 0),
	(10000421, 0, 1942, 0, 0, 0, 0),
	(10000421, 0, 8766, 0, 0, 0, 0);