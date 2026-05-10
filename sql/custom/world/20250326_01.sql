DROP TABLE IF EXISTS `spell_bonus_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_bonus_data` (
  `entry` int unsigned NOT NULL DEFAULT '0',
  `effect` int unsigned NOT NULL DEFAULT '0',
  `sp` float NOT NULL DEFAULT '0',
  `ap` float NOT NULL DEFAULT '0',
  `bv` float NOT NULL DEFAULT '0',
  `scaling_mode` int unsigned NOT NULL DEFAULT '0',
  `comments` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`entry`, `effect`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
