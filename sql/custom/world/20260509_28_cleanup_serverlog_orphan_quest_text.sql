-- Auto-generated Server.log cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- Server.log: quest side text rows for quests removed from quest_template.
DELETE qd FROM `quest_details` qd
LEFT JOIN `quest_template` qt ON qt.ID = qd.ID
WHERE qt.ID IS NULL;


DELETE qor FROM `quest_offer_reward` qor
LEFT JOIN `quest_template` qt ON qt.ID = qor.ID
WHERE qt.ID IS NULL;

DELETE qorl FROM `quest_offer_reward_locale` qorl
LEFT JOIN `quest_template` qt ON qt.ID = qorl.ID
WHERE qt.ID IS NULL;

DELETE qri FROM `quest_request_items` qri
LEFT JOIN `quest_template` qt ON qt.ID = qri.ID
WHERE qt.ID IS NULL;

DELETE qril FROM `quest_request_items_locale` qril
LEFT JOIN `quest_template` qt ON qt.ID = qril.ID
WHERE qt.ID IS NULL;

DELETE qms FROM `quest_mail_sender` qms
LEFT JOIN `quest_template` qt ON qt.ID = qms.QuestId
WHERE qt.ID IS NULL;
