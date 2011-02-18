CREATE TABLE `ping_hdl` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `pe_ro_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`pe_ro_id`,`object_id`),
  KEY `object_type` (`object_type`),
  KEY `object_id` (`object_id`)
);
