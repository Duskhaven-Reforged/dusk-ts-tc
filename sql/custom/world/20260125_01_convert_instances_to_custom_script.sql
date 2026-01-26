-- Convert all EK/Kalimdor 5-man dungeon instances to use custom_script
-- so they can be fully controlled by TSWoW livescripts with hot-reload support.

-- Eastern Kingdoms 5-man dungeons
UPDATE instance_template SET script = "custom_script" WHERE map = 33;  -- Shadowfang Keep
UPDATE instance_template SET script = "custom_script" WHERE map = 34;  -- The Stockade
UPDATE instance_template SET script = "custom_script" WHERE map = 36;  -- Deadmines
UPDATE instance_template SET script = "custom_script" WHERE map = 70;  -- Uldaman
UPDATE instance_template SET script = "custom_script" WHERE map = 90;  -- Gnomeregan
UPDATE instance_template SET script = "custom_script" WHERE map = 109; -- Sunken Temple
UPDATE instance_template SET script = "custom_script" WHERE map = 209; -- Zul'Farrak
UPDATE instance_template SET script = "custom_script" WHERE map = 329; -- Stratholme (base map)

-- Kalimdor 5-man dungeons
UPDATE instance_template SET script = "custom_script" WHERE map = 43;  -- Wailing Caverns
UPDATE instance_template SET script = "custom_script" WHERE map = 349; -- Maraudon
UPDATE instance_template SET script = "custom_script" WHERE map = 389; -- Ragefire Chasm (already done, but included for completeness)
