SET @spell_bonus_data_scaling_mode_sql := (
  SELECT IF(
    COUNT(*) = 0,
    'ALTER TABLE `spell_bonus_data` ADD COLUMN `scaling_mode` int unsigned NOT NULL DEFAULT 0 AFTER `bv`',
    'SELECT 1'
  )
  FROM `INFORMATION_SCHEMA`.`COLUMNS`
  WHERE `TABLE_SCHEMA` = DATABASE()
    AND `TABLE_NAME` = 'spell_bonus_data'
    AND `COLUMN_NAME` = 'scaling_mode'
);

PREPARE spell_bonus_data_scaling_mode_stmt FROM @spell_bonus_data_scaling_mode_sql;
EXECUTE spell_bonus_data_scaling_mode_stmt;
DEALLOCATE PREPARE spell_bonus_data_scaling_mode_stmt;
