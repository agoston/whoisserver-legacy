DROP TABLE IF EXISTS aaa;
CREATE TABLE aaa (
  prefix int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  source varchar(32) NOT NULL default '',
  ripupdate tinyint(3) NOT NULL default '0',
  mirror tinyint(3) NOT NULL default '0',
  comment longblob,
  PRIMARY KEY  (prefix,prefix_length,source)
);

DROP TABLE IF EXISTS aaa6;
CREATE TABLE aaa6 (
  prefix1 int(10) unsigned NOT NULL default '0',
  prefix2 int(10) unsigned NOT NULL default '0',
  prefix3 int(10) unsigned NOT NULL default '0',
  prefix4 int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  source varchar(32) NOT NULL default '',
  ripupdate tinyint(3) unsigned NOT NULL default '0',
  mirror tinyint(3) unsigned NOT NULL default '0',
  comment longblob,
  PRIMARY KEY  (prefix1,prefix2,prefix3,prefix4,prefix_length,source)
);

DROP TABLE IF EXISTS access;
CREATE TABLE access (
  prefix int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  connections int(4) unsigned NOT NULL default '0',
  addr_passes tinyint(3) unsigned NOT NULL default '0',
  denials int(4) unsigned NOT NULL default '0',
  queries int(4) unsigned NOT NULL default '0',
  referrals int(4) unsigned NOT NULL default '0',
  public_objects int(6) unsigned NOT NULL default '0',
  private_objects int(6) unsigned NOT NULL default '0',
  public_bonus double NOT NULL default '0',
  private_bonus double NOT NULL default '0',
  timestamp bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (prefix,prefix_length),
  KEY access_timestamp (timestamp)
);

DROP TABLE IF EXISTS access6;
CREATE TABLE access6 (
  prefix1 int(10) unsigned NOT NULL default '0',
  prefix2 int(10) unsigned NOT NULL default '0',
  prefix3 int(10) unsigned NOT NULL default '0',
  prefix4 int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  connections int(4) unsigned NOT NULL default '0',
  addr_passes tinyint(3) unsigned NOT NULL default '0',
  denials int(4) unsigned NOT NULL default '0',
  queries int(4) unsigned NOT NULL default '0',
  referrals int(4) unsigned NOT NULL default '0',
  public_objects int(6) unsigned NOT NULL default '0',
  private_objects int(6) unsigned NOT NULL default '0',
  public_bonus double NOT NULL default '0',
  private_bonus double NOT NULL default '0',
  timestamp bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (prefix1,prefix2,prefix3,prefix4,prefix_length),
  KEY access_timestamp (timestamp)
);

DROP TABLE IF EXISTS acl;
CREATE TABLE acl (
  prefix int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  maxbonus int(10) NOT NULL default '0',
  maxpublic int(10) default '-1',
  maxdenials int(10) unsigned NOT NULL default '0',
  deny tinyint(3) unsigned NOT NULL default '0',
  trustpass tinyint(3) unsigned NOT NULL default '0',
  threshold tinyint(3) unsigned NOT NULL default '4',
  maxconn tinyint(3) unsigned NOT NULL default '6',
  comment longblob,
  PRIMARY KEY  (prefix,prefix_length)
);

DROP TABLE IF EXISTS acl6;
CREATE TABLE acl6 (
  prefix1 int(10) unsigned NOT NULL default '0',
  prefix2 int(10) unsigned NOT NULL default '0',
  prefix3 int(10) unsigned NOT NULL default '0',
  prefix4 int(10) unsigned NOT NULL default '0',
  prefix_length tinyint(3) unsigned NOT NULL default '0',
  maxbonus int(10) NOT NULL default '0',
  maxpublic int(10) default '-1',
  maxdenials int(10) unsigned NOT NULL default '0',
  deny tinyint(3) unsigned NOT NULL default '0',
  trustpass tinyint(3) unsigned NOT NULL default '0',
  threshold tinyint(3) unsigned NOT NULL default '4',
  maxconn tinyint(3) unsigned NOT NULL default '6',
  comment longblob,
  PRIMARY KEY  (prefix1,prefix2,prefix3,prefix4,prefix_length)
);
