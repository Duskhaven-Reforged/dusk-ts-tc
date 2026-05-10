-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- All-ranks rows on non-ranked spells are invalid; remove the negative all-ranks keys.
DELETE FROM `default.dataset.world.dest`.`spell_proc`
WHERE SpellId IN (-52127, -34914, -20925, -16689, -7302, -1463, -1120, -974, -588, -324);

-- Remove malformed custom proc rows. These either cannot trigger or contain masks unused by their ProcFlags.
DELETE FROM `default.dataset.world.dest`.`spell_proc`
WHERE SpellId IN (84989, 84571, 84567, 84559, 84541, 84499, 83655, 83196, 83128, 82607, 83889, 84918, 84755, 84735, 84733, 84696, 84408, 84316, 82953, 82816, 82271, 81340, 83075, 83062, 82393, 84092);

-- DBC proc flag rows lacking explicit spell_proc entries.
INSERT INTO `default.dataset.world.dest`.`spell_proc` (SpellId, ProcFlags) VALUES
    (8494, 139944),
    (8495, 139944),
    (10191, 139944),
    (10192, 139944),
    (10193, 139944),
    (84928, 664232)
ON DUPLICATE KEY UPDATE ProcFlags = VALUES(ProcFlags);
