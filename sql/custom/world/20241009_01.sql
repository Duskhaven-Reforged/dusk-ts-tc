DELETE FROM spell_group WHERE spell_id = 55095;
DELETE FROM spell_proc WHERE SpellId IN (-44404, 17364);
DELETE FROM spell_script_names WHERE ScriptName IN ('spell_mage_missile_barrage_proc', 'spell_pal_beacon_of_light', 'spell_pal_lay_on_hands', 'spell_pal_light_s_beacon');
