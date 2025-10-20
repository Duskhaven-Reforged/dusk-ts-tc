DROP TABLE IF EXISTS `account_achievement`;
CREATE TABLE `account_achievement` (
  `account` int unsigned NOT NULL,
  `achievement` smallint unsigned NOT NULL,
  `date` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`account`,`achievement`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;