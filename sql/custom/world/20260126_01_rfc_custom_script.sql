-- Set Ragefire Chasm (map 389) to use generic custom_script instance script
-- so dungeon logic can be driven by TS livescripts.

UPDATE instance_template
SET script = 'custom_script'
WHERE map = 389;
