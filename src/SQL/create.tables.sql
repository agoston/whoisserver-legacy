DROP TABLE IF EXISTS person_role;
DROP TABLE IF EXISTS mntner;
DROP TABLE IF EXISTS key_cert;
DROP TABLE IF EXISTS aut_num;
DROP TABLE IF EXISTS route;
DROP TABLE IF EXISTS route6;
DROP TABLE IF EXISTS as_set;
DROP TABLE IF EXISTS route_set;
DROP TABLE IF EXISTS inet_rtr;
DROP TABLE IF EXISTS rtr_set;
DROP TABLE IF EXISTS filter_set;
DROP TABLE IF EXISTS peering_set;
DROP TABLE IF EXISTS as_block;
DROP TABLE IF EXISTS inet6num;
DROP TABLE IF EXISTS inetnum;
DROP TABLE IF EXISTS domain;
DROP TABLE IF EXISTS limerick;
DROP TABLE IF EXISTS irt;
DROP TABLE IF EXISTS last;
DROP TABLE IF EXISTS history;
DROP TABLE IF EXISTS mbrs_by_ref;
DROP TABLE IF EXISTS member_of;
DROP TABLE IF EXISTS admin_c;
DROP TABLE IF EXISTS tech_c;
DROP TABLE IF EXISTS zone_c;
DROP TABLE IF EXISTS mnt_by;
DROP TABLE IF EXISTS mnt_irt;
DROP TABLE IF EXISTS mnt_lower;
DROP TABLE IF EXISTS mnt_domains;
DROP TABLE IF EXISTS mnt_routes;
DROP TABLE IF EXISTS mnt_routes6;
DROP TABLE IF EXISTS author;
DROP TABLE IF EXISTS referral_by;
DROP TABLE IF EXISTS names;
DROP TABLE IF EXISTS auth;
DROP TABLE IF EXISTS auth_override;
DROP TABLE IF EXISTS ifaddr;
DROP TABLE IF EXISTS interface;
DROP TABLE IF EXISTS rev_srv;
DROP TABLE IF EXISTS refer;
DROP TABLE IF EXISTS nserver;
DROP TABLE IF EXISTS sub_dom;
DROP TABLE IF EXISTS e_mail;
DROP TABLE IF EXISTS notify;
DROP TABLE IF EXISTS fingerpr;
DROP TABLE IF EXISTS mnt_nfy;
DROP TABLE IF EXISTS irt_nfy;
DROP TABLE IF EXISTS upd_to;
DROP TABLE IF EXISTS inaddr_arpa;
DROP TABLE IF EXISTS ip6int;
DROP TABLE IF EXISTS serials;
DROP TABLE IF EXISTS nic_hdl;
DROP TABLE IF EXISTS mnt_ref;
DROP TABLE IF EXISTS organisation;
DROP TABLE IF EXISTS ref_nfy;
DROP TABLE IF EXISTS org_name;
DROP TABLE IF EXISTS organisation_id;
DROP TABLE IF EXISTS org;
DROP TABLE IF EXISTS object_order;
DROP TABLE IF EXISTS failed_transaction;
DROP TABLE IF EXISTS transaction_rec;
DROP TABLE IF EXISTS dummy_rec;
DROP TABLE IF EXISTS tid;
DROP TABLE IF EXISTS x509;




#
# Table structure for table 'admin_c'
#

CREATE TABLE admin_c (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  pe_ro_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (pe_ro_id,object_id)
);

#
# Table structure for table 'as_block'
#

CREATE TABLE as_block (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  begin_as int(10) unsigned NOT NULL default '0',
  end_as int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'as_set'
#

CREATE TABLE as_set (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  as_set varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'aut_num'
#

CREATE TABLE aut_num (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  aut_num char(7) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'auth'
#

CREATE TABLE auth (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  auth varchar(90) binary NOT NULL default '',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (auth,object_id)
);

#
# Table structure for table 'auth_override'
#

CREATE TABLE auth_override (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  date int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (date,object_id)
);

#
# Table structure for table 'author'
#

CREATE TABLE author (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  pe_ro_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (pe_ro_id,object_id)
);

#
# Table structure for table 'domain'
#

CREATE TABLE domain (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  domain varchar(254) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'dummy_rec'
#

CREATE TABLE dummy_rec (
  transaction_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (transaction_id,object_id)
);

#
# Table structure for table 'e_mail'
#

CREATE TABLE e_mail (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  e_mail varchar(80) NOT NULL default '',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (e_mail,object_id)
);

#
# Table structure for table 'failed_transaction'
#

CREATE TABLE failed_transaction (
  thread_id int(10) unsigned NOT NULL default '0',
  serial_id int(10) unsigned NOT NULL default '0',
  timestamp int(10) unsigned NOT NULL default '0',
  object longblob NOT NULL,
  PRIMARY KEY  (serial_id)
);

#
# Table structure for table 'filter_set'
#

CREATE TABLE filter_set (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  filter_set varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'history'
#

CREATE TABLE history (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  sequence_id int(10) unsigned NOT NULL default '0',
  timestamp int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  object longblob NOT NULL,
  pkey varchar(254) NOT NULL default '',
  serial int(11) NOT NULL default '0',
  prev_serial int(11) NOT NULL default '0',
  PRIMARY KEY  (object_id,sequence_id)
);

#
# Table structure for table 'ifaddr'
#

CREATE TABLE ifaddr (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  ifaddr int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (ifaddr,object_id)
);

#
# Table structure for table 'interface'
#
# interface_v6_msp: IPv6 address' most significant part
# interface_v6_lsp: IPv6 address' least significant part
# interface_v4:     IPv4 address
#

CREATE TABLE interface (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  interface_v6_msp varchar(20) NOT NULL default '',
  interface_v6_lsp varchar(20) NOT NULL default '',
  interface_v4 int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (interface_v6_lsp,interface_v6_lsp,interface_v4,object_id)
);

#
# Table structure for table 'inaddr_arpa'
#

CREATE TABLE inaddr_arpa (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  prefix int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'inet6num'
#

CREATE TABLE inet6num (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  i6_msb varchar(20) NOT NULL default '',
  i6_lsb varchar(20) NOT NULL default '',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  netname varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'inet_rtr'
#

CREATE TABLE inet_rtr (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  inet_rtr varchar(254) NOT NULL default '',
  local_as varchar(7) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'inetnum'
#

CREATE TABLE inetnum (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  begin_in int(10) unsigned NOT NULL default '0',
  end_in int(10) unsigned NOT NULL default '0',
  netname varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'ip6int'
#

CREATE TABLE ip6int (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  msb varchar(20) NOT NULL default '',
  lsb varchar(20) NOT NULL default '',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'irt'
#

CREATE TABLE irt (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  irt varchar(80) NOT NULL default '0',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'irt_nfy'
#

CREATE TABLE irt_nfy (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  irt_nfy varchar(80) NOT NULL default '',
  PRIMARY KEY  (irt_nfy,object_id)
);

#
# Table structure for table 'key_cert'
#

CREATE TABLE key_cert (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  key_cert varchar(32) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'last'
#

CREATE TABLE last (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL auto_increment,
  sequence_id int(10) unsigned NOT NULL default '1',
  timestamp int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  object longblob NOT NULL,
  pkey varchar(254) NOT NULL default '',
  serial int(11) NOT NULL default '0',
  prev_serial int(11) NOT NULL default '0',
  PRIMARY KEY  (object_id,sequence_id)
);

#
# Table structure for table 'limerick'
#

CREATE TABLE limerick (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  limerick varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'mbrs_by_ref'
#

CREATE TABLE mbrs_by_ref (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'member_of'
#

CREATE TABLE member_of (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  set_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (set_id,object_id)
);

#
# Table structure for table 'mnt_by'
#

CREATE TABLE mnt_by (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'mnt_irt'
#

CREATE TABLE mnt_irt (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  irt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (irt_id,object_id)
);

#
# Table structure for table 'mnt_lower'
#

CREATE TABLE mnt_lower (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'mnt_domains'
#

CREATE TABLE mnt_domains (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'mnt_nfy'
#

CREATE TABLE mnt_nfy (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_nfy varchar(80) NOT NULL default '',
  PRIMARY KEY  (mnt_nfy,object_id)
);

#
# Table structure for table 'mnt_routes'
#

CREATE TABLE mnt_routes (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'mnt_routes6'
#

CREATE TABLE mnt_routes6 (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'mntner'
#

CREATE TABLE mntner (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mntner varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'names'
#

CREATE TABLE names (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  name varchar(64) NOT NULL default '',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (name,object_id)
);

#
# Table structure for table 'nic_hdl'
#

CREATE TABLE nic_hdl (
  thread_id int(10) unsigned NOT NULL default '0',
  range_id int(10) unsigned NOT NULL auto_increment,
  range_start int(10) unsigned NOT NULL default '0',
  range_end int(10) unsigned NOT NULL default '0',
  space char(4) NOT NULL default '',
  source char(10) NOT NULL default '',
  PRIMARY KEY  (range_id,range_start,range_end)
);

#
# Table structure for table 'notify'
#

CREATE TABLE notify (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  notify varchar(80) NOT NULL default '',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (notify,object_id)
);

#
# Table structure for table 'fingerpr'
#

CREATE TABLE fingerpr (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  fingerpr varchar(80) NOT NULL default '',
  PRIMARY KEY  (fingerpr,object_id)
);

#
# Table structure for table 'nserver'
#

CREATE TABLE nserver (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  host varchar(254) NOT NULL default '',
  PRIMARY KEY  (host,object_id)
);

#
# Table structure for table 'object_order'
#

CREATE TABLE object_order (
  object_type int(11) NOT NULL default '0',
  order_code int(11) default NULL,
  PRIMARY KEY  (object_type)
);

#
# Table structure for table 'peering_set'
#

CREATE TABLE peering_set (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  peering_set varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'person_role'
#

CREATE TABLE person_role (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  nic_hdl varchar(30) NOT NULL default '',
  object_type tinyint(4) unsigned NOT NULL default '0',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'refer'
#

CREATE TABLE refer (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  type tinyint(3) unsigned NOT NULL default '0',
  port int(5) unsigned NOT NULL default '43',
  host varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id,host,port,type)
);

#
# Table structure for table 'referral_by'
#

CREATE TABLE referral_by (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'rev_srv'
#

CREATE TABLE rev_srv (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  rev_srv varchar(254) NOT NULL default '',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (rev_srv,object_id)
);

#
# Table structure for table 'route'
#

CREATE TABLE route (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  prefix int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  origin char(7) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'route6'
#

CREATE TABLE route6 (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  r6_msb varchar(20) NOT NULL default '',
  r6_lsb varchar(20) NOT NULL default '',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  origin char(7) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'route_set'
#

CREATE TABLE route_set (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  route_set varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'rtr_set'
#

CREATE TABLE rtr_set (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  rtr_set varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);

#
# Table structure for table 'serials'
#

CREATE TABLE serials (
  thread_id int(10) unsigned NOT NULL default '0',
  serial_id int(11) NOT NULL auto_increment,
  object_id int(10) unsigned NOT NULL default '0',
  sequence_id int(10) unsigned NOT NULL default '0',
  atlast tinyint(4) unsigned NOT NULL default '0',
  operation tinyint(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (serial_id)
);

#
# Table structure for table 'sub_dom'
#

CREATE TABLE sub_dom (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  domain varchar(254) NOT NULL default '',
  PRIMARY KEY  (domain,object_id)
);

#
# Table structure for table 'tech_c'
#

CREATE TABLE tech_c (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  pe_ro_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (pe_ro_id,object_id)
);

#
# Table structure for table 'tid'
#

CREATE TABLE tid (
  ID int(11) NOT NULL auto_increment,
  PRIMARY KEY  (ID)
);

#
# Table structure for table 'transaction_rec'
#

CREATE TABLE transaction_rec (
  transaction_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  sequence_id int(10) unsigned NOT NULL default '1',
  serial_id int(10) unsigned NOT NULL default '1',
  object_type tinyint(3) unsigned NOT NULL default '0',
  save varchar(255) NOT NULL default '',
  error_script blob NOT NULL,
  mode tinyint(4) unsigned NOT NULL default '0',
  succeeded tinyint(4) unsigned NOT NULL default '0',
  action tinyint(4) unsigned NOT NULL default '0',
  status int(10) unsigned NOT NULL default '0',
  clean tinyint(3) NOT NULL default '0',
  PRIMARY KEY  (transaction_id)
);

#
# Table structure for table 'upd_to'
#

CREATE TABLE upd_to (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  upd_to varchar(80) NOT NULL default '',
  PRIMARY KEY  (upd_to,object_id)
);

#
# Table structure for table 'zone_c'
#

CREATE TABLE zone_c (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  pe_ro_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (pe_ro_id,object_id)
);

#
# Table structure for table 'x509'
#

CREATE TABLE x509 (
  thread_id int(11) NOT NULL default '0',
  keycert_id int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (keycert_id)
);

#
# Table structure for table 'mnt_ref'
#

CREATE TABLE mnt_ref (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);

#
# Table structure for table 'organisation'
#

CREATE TABLE organisation (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  organisation varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (organisation,object_id)
);

#
# Table structure for table 'ref_nfy'
#

CREATE TABLE ref_nfy (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  ref_nfy varchar(80) NOT NULL default '',
  PRIMARY KEY  (ref_nfy,object_id)
);

#
# Table structure for table 'org_name'
#

CREATE TABLE org_name (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  name varchar(64) NOT NULL default '',
  PRIMARY KEY  (name,object_id)
);


#
# Table structure for table 'organisation_id'
# ( We don't need "range_start" as opposed to
#   person/role NIC handles, because we will not
#   reuse org IDs).

CREATE TABLE organisation_id (
  thread_id int(10) unsigned NOT NULL default '0',
  range_id int(10) unsigned NOT NULL auto_increment,
  range_end int(10) unsigned NOT NULL default '0',
  space char(4) NOT NULL default '',
  source char(10) NOT NULL default '',
  PRIMARY KEY  (range_id,range_end)
);

#
# Table structure for table 'org'
#

CREATE TABLE org (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  org_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (org_id,object_id)
);


# make some indexes

ALTER TABLE serials ADD INDEX object (object_id, sequence_id);
ALTER TABLE serials ADD INDEX (thread_id);

ALTER TABLE nic_hdl ADD INDEX (range_start);
ALTER TABLE nic_hdl ADD INDEX (range_end);
ALTER TABLE nic_hdl ADD INDEX (space, source(5));
ALTER TABLE nic_hdl ADD INDEX (thread_id);
ALTER TABLE organisation_id  ADD INDEX (range_end);
ALTER TABLE organisation_id  ADD INDEX (space, source);
ALTER TABLE organisation_id  ADD INDEX (thread_id);

ALTER TABLE failed_transaction ADD INDEX (thread_id);

ALTER TABLE last ADD INDEX (pkey);

ALTER TABLE history ADD INDEX (pkey);

INSERT mntner SET object_id=0, mntner='ANY', dummy=1;



INSERT INTO object_order VALUES (10,50); # person
INSERT INTO object_order VALUES (11,49); # role
INSERT INTO object_order VALUES (0,7);
INSERT INTO object_order VALUES (1,9);
INSERT INTO object_order VALUES (2,8);
INSERT INTO object_order VALUES (3,30);
INSERT INTO object_order VALUES (4,15);
INSERT INTO object_order VALUES (5,6); #inet6num
INSERT INTO object_order VALUES (6,5); #inetnum
INSERT INTO object_order VALUES (7,45);
INSERT INTO object_order VALUES (8,35);
INSERT INTO object_order VALUES (9,40);
INSERT INTO object_order VALUES (19,11); #route6
INSERT INTO object_order VALUES (12,10); #route
INSERT INTO object_order VALUES (13,12); #route-set
INSERT INTO object_order VALUES (14,21);
INSERT INTO object_order VALUES (15,22);
INSERT INTO object_order VALUES (16,23);
INSERT INTO object_order VALUES (17,41);
INSERT INTO object_order VALUES (18,48); # organisation
