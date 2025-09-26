-- `default.dataset.world.dest`.spell_custom_attr definition
DROP TABLE IF EXISTS `spell_custom_attr`;
CREATE TABLE `spell_custom_attr` (
  `entry` int unsigned NOT NULL DEFAULT '0' COMMENT 'spell id',
  `attributes0` int unsigned NOT NULL DEFAULT '0',
  `attributes1` int unsigned NOT NULL DEFAULT '0',
  `attributes2` int unsigned NOT NULL DEFAULT '0',
  `attributes3` int unsigned NOT NULL DEFAULT '0',
  `__tswow_tag` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='SpellInfo custom attributes';