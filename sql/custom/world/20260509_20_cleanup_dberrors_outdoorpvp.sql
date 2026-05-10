-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Outdoor PvP templates expected by OutdoorPvPMgr for type IDs 1..4.
INSERT INTO `default.dataset.world.dest`.`outdoorpvp_template` (TypeId, ScriptName, comment) VALUES
    (1, 'outdoorpvp_hp', 'Hellfire Peninsula'),
    (2, 'outdoorpvp_na', 'Nagrand'),
    (3, 'outdoorpvp_tf', 'Terokkar Forest'),
    (4, 'outdoorpvp_zm', 'Zangarmarsh')
ON DUPLICATE KEY UPDATE ScriptName = VALUES(ScriptName), comment = VALUES(comment);
