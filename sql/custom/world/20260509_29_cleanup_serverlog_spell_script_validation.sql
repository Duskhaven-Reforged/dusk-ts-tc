-- Auto-generated Server.log cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Server.log: DB-linked spell scripts whose target spell data no longer matches this client DBC set.
DELETE FROM `spell_script_names`
WHERE (spell_id, ScriptName) IN (
    (15337, 'spell_pri_improved_spirit_tap'),
    (15338, 'spell_pri_improved_spirit_tap'),
    (27285, 'spell_warl_seed_of_corruption'),
    (43365, 'spell_fjord_the_cleansing_shrine_cast'),
    (43723, 'spell_item_demon_broiled_surprise'),
    (47833, 'spell_warl_seed_of_corruption'),
    (47834, 'spell_warl_seed_of_corruption'),
    (49028, 'spell_dk_dancing_rune_weapon'),
    (50365, 'spell_dk_improved_blood_presence'),
    (50371, 'spell_dk_improved_blood_presence'),
    (50384, 'spell_dk_improved_frost_presence'),
    (50385, 'spell_dk_improved_frost_presence'),
    (50391, 'spell_dk_improved_unholy_presence'),
    (50392, 'spell_dk_improved_unholy_presence'),
    (51961, 'spell_item_chicken_cover'),
    (53099, 'spell_quest_portal_with_condition'),
    (53569, 'spell_pal_infusion_of_light'),
    (53576, 'spell_pal_infusion_of_light'),
    (53817, 'spell_sha_maelstrom_weapon'),
    (56342, 'spell_hun_lock_and_load'),
    (56343, 'spell_hun_lock_and_load'),
    (56344, 'spell_hun_lock_and_load'),
    (57896, 'spell_quest_portal_with_condition'),
    (58418, 'spell_quest_portal_with_condition'),
    (58420, 'spell_quest_portal_with_condition'),
    (59064, 'spell_quest_portal_with_condition'),
    (59065, 'spell_quest_portal_with_condition'),
    (59439, 'spell_quest_portal_with_condition'),
    (60900, 'spell_quest_portal_with_condition'),
    (60940, 'spell_quest_portal_with_condition'),
    (63521, 'spell_pal_guarded_by_the_light')
);
