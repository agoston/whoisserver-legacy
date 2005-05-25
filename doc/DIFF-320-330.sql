-- $Id$
-- Sql script to patch a 3.2.0 release database to be compatible with
-- 3.3.0 release.

CREATE TABLE abuse_mailbox (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  abuse_mailbox varchar(80) NOT NULL default '',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (abuse_mailbox,object_id)
);
CREATE TABLE fingerpr (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  fingerpr varchar(80) NOT NULL default '',
  PRIMARY KEY  (fingerpr,object_id)
);
CREATE TABLE form (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  form_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (form_id,object_id),
  KEY object_id (object_id),
  KEY object_type (object_type)
);
CREATE TABLE interface (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  interface_v6_msp varchar(20) NOT NULL default '',
  interface_v6_lsp varchar(20) NOT NULL default '',
  interface_v4 int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (interface_v6_lsp,interface_v6_lsp,interface_v4,object_id)
);
CREATE TABLE mnt_domains (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);
CREATE TABLE mnt_ref (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);
CREATE TABLE mnt_routes6 (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  mnt_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (mnt_id,object_id)
);
CREATE TABLE org (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  org_id int(10) unsigned NOT NULL default '0',
  object_type tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (org_id,object_id)
);
CREATE TABLE org_name (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  name varchar(64) NOT NULL default '',
  PRIMARY KEY  (name,object_id)
);
CREATE TABLE organisation (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  organisation varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (organisation,object_id)
);
CREATE TABLE organisation_id (
  thread_id int(10) unsigned NOT NULL default '0',
  range_id int(10) unsigned NOT NULL auto_increment,
  range_end int(10) unsigned NOT NULL default '0',
  space char(4) NOT NULL default '',
  source char(10) NOT NULL default '',
  PRIMARY KEY  (range_id,range_end),
  KEY range_end (range_end),
  KEY space (space,source),
  KEY thread_id (thread_id)
);
CREATE TABLE poem (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  poem varchar(80) NOT NULL default '',
  PRIMARY KEY  (object_id),
  KEY poem (poem),
  KEY thread_id (thread_id)
);
CREATE TABLE poetic_form (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  poetic_form varchar(80) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (poetic_form),
  KEY poetic_form (poetic_form),
  KEY thread_id (thread_id)
);
CREATE TABLE ref_nfy (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  ref_nfy varchar(80) NOT NULL default '',
  PRIMARY KEY  (ref_nfy,object_id)
);
CREATE TABLE route6 (
  thread_id int(11) NOT NULL default '0',
  object_id int(10) unsigned NOT NULL default '0',
  r6_msb varchar(20) NOT NULL default '',
  r6_lsb varchar(20) NOT NULL default '',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  origin varchar(7) NOT NULL default '',
  dummy tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (object_id)
);
CREATE TABLE x509 (
  thread_id int(11) NOT NULL default '0',
  keycert_id int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (keycert_id)
);

INSERT INTO object_order VALUES (19,11);
UPDATE object_order set order_code = 12 where object_type = 13;
INSERT INTO object_order VALUES (18,48);
INSERT INTO object_order VALUES (20,37);
INSERT INTO object_order VALUES (21,36);
