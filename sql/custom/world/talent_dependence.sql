CREATE TABLE `talent_dependence` (
  `spell` mediumint unsigned NOT NULL,
  `dependency` mediumint unsigned NOT NULL,
  PRIMARY KEY (`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;