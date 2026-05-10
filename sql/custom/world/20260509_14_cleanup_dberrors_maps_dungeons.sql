-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Remove disables for maps/battlegrounds that no longer exist in the client data.
DELETE FROM `default.dataset.world.dest`.`disables`
WHERE entry IN (585, 580, 568, 564, 560, 557, 555, 552, 550, 547, 546, 545, 544, 543, 542, 540, 534, 533, 532, 531, 509, 469, 409, 309, 269, 249, 739, 654, 572);

-- Remove LFG templates for dungeon map/dungeon ids no longer valid in this client data set.
DELETE FROM `default.dataset.world.dest`.`lfg_dungeon_template`
WHERE dungeonId IN (255, 272, 273, 285, 286, 287, 288, 2753, 753)
   OR name IN ('Tempest Keep',
    'Serpentshrine Cavern',
    'Black Temple',
    'The Culling of Stratholme',
    'Halls of Lightning',
    'Magtheridon''s Lair',
    'Auchenai Crypts',
    'The Eye of Eternity',
    'Naxxramas',
    'Karazhan',
    'Gruul''s Lair',
    'Mana-Tombs',
    'Sethekk Halls',
    'Shadow Labyrinth',
    'The Escape From Durnholde',
    'The Steamvault',
    'Underbog',
    'Shattered Halls',
    'The Arcatraz',
    'The Botanica',
    'The Mechanar',
    'Hyjal Past',
    'Zul''Aman',
    'The Sunwell',
    'Magisters'' Terrace',
    'Utgarde Pinnacle',
    'The Oculus',
    'Halls of Stone',
    'Drak''Tharon Keep',
    'Gundrak',
    'Ahn''kahet: The Old Kingdom',
    'Violet Hold',
    'The Obsidian Sanctum',
    'The Nexus',
    'Vault of Archavon',
    'Azjol-Nerub',
    'Utgarde Keep',
    'Ulduar',
    'Trial of the Crusader',
    'Trial of the Grand Crusader',
    'Trial of the Champion',
    'The Forge of Souls',
    'Pit of Saron',
    'Icecrown Citadel',
    'Ruby Sanctum');

-- Remove dungeon teleport rows targeting missing maps from the same retired-map set.
DELETE FROM `default.dataset.world.dest`.`areatrigger_teleport`
WHERE target_map IN (585, 580, 568, 564, 560, 557, 555, 552, 550, 547, 546, 545, 544, 543, 542, 540, 534, 533, 532, 531, 509, 469, 409, 309, 269, 249, 739, 654, 572, 548, 595, 602, 558, 616, 565, 556, 553, 554, 575, 578, 599, 600, 604, 619, 608, 615, 576, 624, 601, 574, 603, 649, 650, 632, 658, 631, 724);
