-- Auto-generated Server.log cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Server.log: quest side text rows for quests removed from quest_template.
DELETE qd FROM `default.dataset.world.dest`.`quest_details` qd
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.ID = qd.ID
WHERE qt.ID IS NULL;


DELETE qor FROM `default.dataset.world.dest`.`quest_offer_reward` qor
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.ID = qor.ID
WHERE qt.ID IS NULL;

DELETE qorl FROM `default.dataset.world.dest`.`quest_offer_reward_locale` qorl
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.ID = qorl.ID
WHERE qt.ID IS NULL;

DELETE qri FROM `default.dataset.world.dest`.`quest_request_items` qri
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.ID = qri.ID
WHERE qt.ID IS NULL;

DELETE qril FROM `default.dataset.world.dest`.`quest_request_items_locale` qril
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.ID = qril.ID
WHERE qt.ID IS NULL;

DELETE qms FROM `default.dataset.world.dest`.`quest_mail_sender` qms
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.ID = qms.QuestId
WHERE qt.ID IS NULL;
