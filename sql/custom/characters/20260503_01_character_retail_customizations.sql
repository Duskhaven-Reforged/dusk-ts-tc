CREATE TABLE IF NOT EXISTS `character_retail_customizations` (
  `guid` int unsigned NOT NULL,
  `optionId` int unsigned NOT NULL,
  `choiceId` int unsigned NOT NULL,
  PRIMARY KEY (`guid`, `optionId`),
  KEY `idx_character_retail_customizations_choice` (`choiceId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
