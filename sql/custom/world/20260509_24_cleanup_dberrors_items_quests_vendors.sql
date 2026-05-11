-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

UPDATE `item_template` SET stat_type3 = 0, stat_value3 = 0 WHERE entry = 20142;

UPDATE `item_template` SET stat_type4 = 0, stat_value4 = 0 WHERE entry = 20142;

UPDATE `item_template` SET stat_type5 = 0, stat_value5 = 0 WHERE entry = 20142;

UPDATE `item_template` SET stat_type6 = 0, stat_value6 = 0 WHERE entry = 20142;

UPDATE `item_template` SET stat_type7 = 0, stat_value7 = 0 WHERE entry = 20142;

UPDATE `item_template` SET stat_type8 = 0, stat_value8 = 0 WHERE entry = 20142;

UPDATE `item_template` SET stat_type3 = 0, stat_value3 = 0 WHERE entry = 20522;

UPDATE `item_template` SET stat_type3 = 0, stat_value3 = 0 WHERE entry = 20524;

UPDATE `item_template` SET stat_type4 = 0, stat_value4 = 0 WHERE entry = 20524;

UPDATE `item_template` SET stat_type3 = 0, stat_value3 = 0 WHERE entry = 21614;

UPDATE `item_template` SET stat_type2 = 0, stat_value2 = 0 WHERE entry = 22230;

UPDATE `item_template` SET stat_type5 = 0, stat_value5 = 0 WHERE entry = 23363;

UPDATE `item_template` SET stat_type6 = 0, stat_value6 = 0 WHERE entry = 23363;

UPDATE `item_template` SET sheath = 2 WHERE entry = 60616;

UPDATE `item_template` SET sheath = 0 WHERE entry = 60618;

UPDATE `item_template` SET sheath = 0 WHERE entry = 60628;

UPDATE `item_template` SET sheath = 0 WHERE entry = 61147;

UPDATE `item_template` SET BuyCount = 1 WHERE entry IN (60934, 61201);

UPDATE `item_template` SET AllowableClass = -1 WHERE entry IN (61021);

DELETE FROM `item_set_names` WHERE entry IN (61174, 61171, 61170, 61169, 61168, 61167, 61166, 61165, 61151, 61150);

UPDATE `quest_template` SET RewardChoiceItemID1 = 0, RewardChoiceItemQuantity1 = 0 WHERE ID = 26426 AND RewardChoiceItemID1 = 90003;

UPDATE `quest_template` SET RewardChoiceItemID2 = 0, RewardChoiceItemQuantity2 = 0 WHERE ID = 26426 AND RewardChoiceItemID2 = 90004;

UPDATE `quest_template` SET RewardChoiceItemID3 = 0, RewardChoiceItemQuantity3 = 0 WHERE ID = 26426 AND RewardChoiceItemID3 = 90005;

UPDATE `quest_template` SET RewardChoiceItemID1 = 0, RewardChoiceItemQuantity1 = 0 WHERE ID = 26121 AND RewardChoiceItemID1 = 90003;

UPDATE `quest_template` SET RewardChoiceItemID2 = 0, RewardChoiceItemQuantity2 = 0 WHERE ID = 26121 AND RewardChoiceItemID2 = 90004;

UPDATE `quest_template` SET RewardChoiceItemID3 = 0, RewardChoiceItemQuantity3 = 0 WHERE ID = 26121 AND RewardChoiceItemID3 = 90005;

DELETE FROM `npc_vendor` WHERE entry = 47186 AND item = 13322 AND ExtendedCost = 400;

DELETE FROM `game_event_npc_vendor` WHERE guid = 47186 AND item = 13322 AND ExtendedCost = 400;

DELETE FROM `npc_vendor` WHERE entry = 47186 AND item = 4597 AND ExtendedCost = 4;

DELETE FROM `game_event_npc_vendor` WHERE guid = 47186 AND item = 4597 AND ExtendedCost = 4;

DELETE FROM `npc_vendor` WHERE entry = 47186 AND item = 14730 AND ExtendedCost = 4;

DELETE FROM `game_event_npc_vendor` WHERE guid = 47186 AND item = 14730 AND ExtendedCost = 4;

DELETE FROM `npc_vendor` WHERE entry = 47186 AND item = 45981 AND ExtendedCost = 4;

DELETE FROM `game_event_npc_vendor` WHERE guid = 47186 AND item = 45981 AND ExtendedCost = 4;

DELETE FROM `npc_vendor` WHERE entry = 47186 AND item = 22206 AND ExtendedCost = 4;

DELETE FROM `game_event_npc_vendor` WHERE guid = 47186 AND item = 22206 AND ExtendedCost = 4;

DELETE FROM `npc_vendor` WHERE entry = 47186 AND item = 60628 AND ExtendedCost = 4;

DELETE FROM `game_event_npc_vendor` WHERE guid = 47186 AND item = 60628 AND ExtendedCost = 4;
