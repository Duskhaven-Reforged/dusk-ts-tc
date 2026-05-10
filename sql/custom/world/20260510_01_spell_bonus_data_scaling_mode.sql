ALTER TABLE `spell_bonus_data`
  ADD COLUMN IF NOT EXISTS `scaling_mode` int unsigned NOT NULL DEFAULT '0' AFTER `bv`;
