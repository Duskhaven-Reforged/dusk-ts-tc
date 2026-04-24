DROP TABLE IF EXISTS `forge_spell_jump_charge_params`;
DROP TABLE IF EXISTS `jump_charge_params`;
CREATE TABLE `jump_charge_params` (
  `id` int NOT NULL,
  `speed` float NOT NULL DEFAULT '42',
  `treatSpeedAsMoveTimeSeconds` tinyint(1) NOT NULL DEFAULT '0',
  `unlimitedSpeed` tinyint(1) NOT NULL DEFAULT '0',
  `minHeight` float DEFAULT NULL,
  `maxHeight` float DEFAULT NULL,
  `spellVisualId` int DEFAULT NULL,
  `progressCurveId` int DEFAULT NULL,
  `parabolicCurveId` int DEFAULT NULL,
  `triggerSpellId` int DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
