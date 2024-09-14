DROP TABLE IF EXISTS `custom_druid_barbershop`;
CREATE TABLE IF NOT EXISTS `custom_druid_barbershop` (
  `guid` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `type` char(200) NOT NULL DEFAULT '',
  `name` char(200) NOT NULL DEFAULT '',
  `display` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `npc` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `racemask` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `SpellId` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `ReqSpellId` mediumint(9) NOT NULL DEFAULT '0',
  `path` char(200) NOT NULL DEFAULT '',
  `ReqItemID` mediumint(9) NOT NULL DEFAULT '0',
  `ReqItemCant` mediumint(9) NOT NULL DEFAULT '0',
  `Comentario` char(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`,`display`,`SpellId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;
