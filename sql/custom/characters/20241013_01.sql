DROP TABLE IF EXISTS `characterlootlockouts`;
CREATE TABLE `characterlootlockouts` (
  `guid` bigint unsigned NOT NULL,
  `creature` int unsigned NOT NULL,
  `diff` tinyint unsigned NOT NULL,
  `creatureguid` bigint unsigned DEFAULT NULL,
  `reset` bigint unsigned DEFAULT NULL,
  PRIMARY KEY (`guid`,`creature`,`diff`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;