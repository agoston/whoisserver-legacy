DROP TABLE IF EXISTS `abuse_mailbox`;
CREATE TABLE `abuse_mailbox` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `abuse_mailbox` varchar(80) NOT NULL default '',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`abuse_mailbox`,`object_id`),
  KEY `abuse_mailbox_object_id_index` (`object_id`)
);
DROP TABLE IF EXISTS `admin_c`;
CREATE TABLE `admin_c` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `pe_ro_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`pe_ro_id`,`object_id`),
  KEY `object_type` (`object_type`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `as_block`;
CREATE TABLE `as_block` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `begin_as` int(10) unsigned NOT NULL default '0',
  `end_as` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `thread_id` (`thread_id`),
  KEY `begin_as` (`begin_as`),
  KEY `end_as` (`end_as`)
);
DROP TABLE IF EXISTS `as_set`;
CREATE TABLE `as_set` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `as_set` varchar(80) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `as_set` (`as_set`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `aut_num`;
CREATE TABLE `aut_num` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `aut_num` char(13) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `aut_num` (`aut_num`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `auth`;
CREATE TABLE `auth` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `auth` varchar(90) character set latin1 collate latin1_bin NOT NULL default '',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`auth`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `auth_override`;
CREATE TABLE `auth_override` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `date` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`date`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `author`;
CREATE TABLE `author` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `pe_ro_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`pe_ro_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `domain`;
CREATE TABLE `domain` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `domain` varchar(254) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `domain` (`domain`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `ds_rdata`;
CREATE TABLE `ds_rdata` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `ds_rdata` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`ds_rdata`,`object_id`)
);
DROP TABLE IF EXISTS `dummy_rec`;
CREATE TABLE `dummy_rec` (
  `transaction_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`transaction_id`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `e_mail`;
CREATE TABLE `e_mail` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `e_mail` varchar(80) NOT NULL default '',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`e_mail`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `failed_transaction`;
CREATE TABLE `failed_transaction` (
  `thread_id` int(10) unsigned NOT NULL default '0',
  `serial_id` int(10) unsigned NOT NULL default '0',
  `timestamp` int(10) unsigned NOT NULL default '0',
  `object` longblob NOT NULL,
  PRIMARY KEY  (`serial_id`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `filter_set`;
CREATE TABLE `filter_set` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `filter_set` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `filter_set` (`filter_set`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `fingerpr`;
CREATE TABLE `fingerpr` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `fingerpr` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`fingerpr`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `form`;
CREATE TABLE `form` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `form_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`form_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `history`;
CREATE TABLE `history` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `sequence_id` int(10) unsigned NOT NULL default '0',
  `timestamp` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  `object` longblob NOT NULL,
  `pkey` varchar(254) NOT NULL default '',
  `serial` int(11) NOT NULL default '0',
  `prev_serial` int(11) NOT NULL default '0',
  PRIMARY KEY  (`object_id`,`sequence_id`),
  KEY `history_pkey` (`pkey`)
);
DROP TABLE IF EXISTS `ifaddr`;
CREATE TABLE `ifaddr` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `ifaddr` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ifaddr`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `inaddr_arpa`;
CREATE TABLE `inaddr_arpa` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `prefix` int(10) unsigned NOT NULL default '0',
  `prefix_length` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`object_id`)
);
DROP TABLE IF EXISTS `inet6num`;
CREATE TABLE `inet6num` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `i6_msb` varchar(20) NOT NULL default '',
  `i6_lsb` varchar(20) NOT NULL default '',
  `prefix_length` tinyint(3) unsigned NOT NULL default '0',
  `netname` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `netname` (`netname`),
  KEY `i6_msb` (`i6_msb`),
  KEY `i6_lsb` (`i6_lsb`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `inet_rtr`;
CREATE TABLE `inet_rtr` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `inet_rtr` varchar(254) NOT NULL default '',
  `local_as` varchar(13) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `inet_rtr` (`inet_rtr`),
  KEY `local_as` (`local_as`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `inetnum`;
CREATE TABLE `inetnum` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `begin_in` int(10) unsigned NOT NULL default '0',
  `end_in` int(10) unsigned NOT NULL default '0',
  `netname` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `netname` (`netname`),
  KEY `begin_in` (`begin_in`),
  KEY `end_in` (`end_in`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `interface`;
CREATE TABLE `interface` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `interface_v6_msp` varchar(20) NOT NULL default '',
  `interface_v6_lsp` varchar(20) NOT NULL default '',
  `interface_v4` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`interface_v6_msp`,`interface_v6_lsp`,`interface_v4`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `ip6int`;
CREATE TABLE `ip6int` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `msb` varchar(20) NOT NULL default '',
  `lsb` varchar(20) NOT NULL default '',
  `prefix_length` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`object_id`)
);
DROP TABLE IF EXISTS `irt`;
CREATE TABLE `irt` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `irt` varchar(80) NOT NULL default '0',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `irt` (`irt`)
);
DROP TABLE IF EXISTS `irt_nfy`;
CREATE TABLE `irt_nfy` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `irt_nfy` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`irt_nfy`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `key_cert`;
CREATE TABLE `key_cert` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `key_cert` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `key_cert` (`key_cert`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `last`;
CREATE TABLE `last` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL auto_increment,
  `sequence_id` int(10) unsigned NOT NULL default '1',
  `timestamp` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  `object` longblob NOT NULL,
  `pkey` varchar(254) NOT NULL default '',
  `serial` int(11) NOT NULL default '0',
  `prev_serial` int(11) NOT NULL default '0',
  PRIMARY KEY  (`object_id`,`sequence_id`),
  KEY `last_pkey` (`pkey`),
  KEY `object_type_index` (`object_type`)
);
DROP TABLE IF EXISTS `limerick`;
CREATE TABLE `limerick` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `limerick` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `limerick` (`limerick`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `mbrs_by_ref`;
CREATE TABLE `mbrs_by_ref` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `member_of`;
CREATE TABLE `member_of` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `set_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`set_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `mnt_by`;
CREATE TABLE `mnt_by` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `mnt_domains`;
CREATE TABLE `mnt_domains` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `mnt_irt`;
CREATE TABLE `mnt_irt` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `irt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`irt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `mnt_lower`;
CREATE TABLE `mnt_lower` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `mnt_nfy`;
CREATE TABLE `mnt_nfy` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_nfy` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`mnt_nfy`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `mnt_ref`;
CREATE TABLE `mnt_ref` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `mnt_routes`;
CREATE TABLE `mnt_routes` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `mnt_routes6`;
CREATE TABLE `mnt_routes6` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `mntner`;
CREATE TABLE `mntner` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mntner` varchar(80) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `mntner` (`mntner`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `names`;
CREATE TABLE `names` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `name` varchar(64) NOT NULL default '',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`name`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `nic_hdl`;
CREATE TABLE `nic_hdl` (
  `thread_id` int(10) unsigned NOT NULL default '0',
  `range_id` int(10) unsigned NOT NULL auto_increment,
  `range_start` int(10) unsigned NOT NULL default '0',
  `range_end` int(10) unsigned NOT NULL default '0',
  `space` char(4) NOT NULL default '',
  `source` char(10) NOT NULL default '',
  PRIMARY KEY  (`range_id`,`range_start`,`range_end`),
  KEY `range_start` (`range_start`),
  KEY `range_end` (`range_end`),
  KEY `space` (`space`,`source`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `notify`;
CREATE TABLE `notify` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `notify` varchar(80) NOT NULL default '',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`notify`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `nserver`;
CREATE TABLE `nserver` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `host` varchar(254) NOT NULL default '',
  PRIMARY KEY  (`host`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `object_order`;
CREATE TABLE `object_order` (
  `object_type` int(11) NOT NULL default '0',
  `order_code` int(11) default NULL,
  PRIMARY KEY  (`object_type`)
);
DROP TABLE IF EXISTS `org`;
CREATE TABLE `org` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `org_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`org_id`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `org_name`;
CREATE TABLE `org_name` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `name` varchar(64) NOT NULL default '',
  PRIMARY KEY  (`name`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `organisation`;
CREATE TABLE `organisation` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `organisation` varchar(80) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`organisation`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `organisation_id`;
CREATE TABLE `organisation_id` (
  `thread_id` int(10) unsigned NOT NULL default '0',
  `range_id` int(10) unsigned NOT NULL auto_increment,
  `range_end` int(10) unsigned NOT NULL default '0',
  `space` char(4) NOT NULL default '',
  `source` char(10) NOT NULL default '',
  PRIMARY KEY  (`range_id`,`range_end`),
  KEY `range_end` (`range_end`),
  KEY `space` (`space`,`source`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `peering_set`;
CREATE TABLE `peering_set` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `peering_set` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `peering_set` (`peering_set`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `person_role`;
CREATE TABLE `person_role` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `nic_hdl` varchar(30) NOT NULL default '',
  `object_type` tinyint(4) unsigned NOT NULL default '0',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `nic_hdl` (`nic_hdl`),
  KEY `object_type` (`object_type`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `ping_hdl`;
CREATE TABLE `ping_hdl` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `pe_ro_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`pe_ro_id`,`object_id`),
  KEY `object_type` (`object_type`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `poem`;
CREATE TABLE `poem` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `poem` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`),
  KEY `poem` (`poem`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `poetic_form`;
CREATE TABLE `poetic_form` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `poetic_form` varchar(80) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`poetic_form`),
  KEY `poetic_form` (`poetic_form`),
  KEY `thread_id` (`thread_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `ref_nfy`;
CREATE TABLE `ref_nfy` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `ref_nfy` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`ref_nfy`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `refer`;
CREATE TABLE `refer` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `port` int(5) unsigned NOT NULL default '43',
  `host` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`object_id`,`host`,`port`,`type`)
);
DROP TABLE IF EXISTS `referral_by`;
CREATE TABLE `referral_by` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `mnt_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mnt_id`,`object_id`),
  KEY `object_id` (`object_id`),
  KEY `object_type` (`object_type`)
);
DROP TABLE IF EXISTS `rev_srv`;
CREATE TABLE `rev_srv` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `rev_srv` varchar(254) NOT NULL default '',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`rev_srv`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `route`;
CREATE TABLE `route` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `prefix` int(10) unsigned NOT NULL default '0',
  `prefix_length` tinyint(3) unsigned NOT NULL default '0',
  `origin` varchar(13) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `origin` (`origin`,`prefix`,`prefix_length`)
);
DROP TABLE IF EXISTS `route6`;
CREATE TABLE `route6` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `r6_msb` varchar(20) NOT NULL default '',
  `r6_lsb` varchar(20) NOT NULL default '',
  `prefix_length` tinyint(3) unsigned NOT NULL default '0',
  `origin` varchar(13) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `origin` (`origin`,`r6_msb`,`r6_lsb`,`prefix_length`),
  KEY `r6_msb` (`r6_msb`),
  KEY `r6_lsb` (`r6_lsb`)
);
DROP TABLE IF EXISTS `route_set`;
CREATE TABLE `route_set` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `route_set` varchar(80) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `route_set` (`route_set`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `rtr_set`;
CREATE TABLE `rtr_set` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `rtr_set` varchar(80) NOT NULL default '',
  `dummy` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`object_id`),
  KEY `rtr_set` (`rtr_set`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `serials`;
CREATE TABLE `serials` (
  `thread_id` int(10) unsigned NOT NULL default '0',
  `serial_id` int(11) NOT NULL auto_increment,
  `object_id` int(10) unsigned NOT NULL default '0',
  `sequence_id` int(10) unsigned NOT NULL default '0',
  `atlast` tinyint(4) unsigned NOT NULL default '0',
  `operation` tinyint(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (`serial_id`),
  KEY `object` (`object_id`,`sequence_id`),
  KEY `thread_id` (`thread_id`)
);
DROP TABLE IF EXISTS `sub_dom`;
CREATE TABLE `sub_dom` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `domain` varchar(254) NOT NULL default '',
  PRIMARY KEY  (`domain`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `tech_c`;
CREATE TABLE `tech_c` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `pe_ro_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`pe_ro_id`,`object_id`),
  KEY `object_type` (`object_type`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `tid`;
CREATE TABLE `tid` (
  `ID` int(11) NOT NULL auto_increment,
  PRIMARY KEY  (`ID`)
);
DROP TABLE IF EXISTS `transaction_rec`;
CREATE TABLE `transaction_rec` (
  `transaction_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `sequence_id` int(10) unsigned NOT NULL default '1',
  `serial_id` int(10) unsigned NOT NULL default '1',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  `save` varchar(255) NOT NULL default '',
  `error_script` blob NOT NULL,
  `mode` tinyint(4) unsigned NOT NULL default '0',
  `succeeded` tinyint(4) unsigned NOT NULL default '0',
  `action` tinyint(4) unsigned NOT NULL default '0',
  `status` int(10) unsigned NOT NULL default '0',
  `clean` tinyint(3) NOT NULL default '0',
  PRIMARY KEY  (`transaction_id`)
);
DROP TABLE IF EXISTS `upd_to`;
CREATE TABLE `upd_to` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `upd_to` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`upd_to`,`object_id`),
  KEY `object_id` (`object_id`)
);
DROP TABLE IF EXISTS `x509`;
CREATE TABLE `x509` (
  `thread_id` int(11) NOT NULL default '0',
  `keycert_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`keycert_id`)
);
DROP TABLE IF EXISTS `zone_c`;
CREATE TABLE `zone_c` (
  `thread_id` int(11) NOT NULL default '0',
  `object_id` int(10) unsigned NOT NULL default '0',
  `pe_ro_id` int(10) unsigned NOT NULL default '0',
  `object_type` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`pe_ro_id`,`object_id`),
  KEY `object_type` (`object_type`),
  KEY `object_id` (`object_id`)
);
