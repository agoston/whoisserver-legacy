
CREATE DATABASE RIPADMIN;

USE RIPADMIN;

DROP TABLE IF EXISTS aaa;

#
# Table structure for table 'aaa'
#
CREATE TABLE aaa (
  prefix int(10) unsigned DEFAULT '0' NOT NULL,
  prefix_length tinyint(3) unsigned DEFAULT '0' NOT NULL,
  source varchar(32) DEFAULT '' NOT NULL,
  ripupdate tinyint(3) DEFAULT '0' NOT NULL,
  netupdate tinyint(3) DEFAULT '0' NOT NULL,
  mirror tinyint(3) DEFAULT '0' NOT NULL,
  comment longblob,
  PRIMARY KEY (prefix,prefix_length,source)
);



DROP TABLE IF EXISTS acl;

#
# Table structure for table 'acl'
#
CREATE TABLE acl (
  prefix int(10) unsigned DEFAULT '0' NOT NULL,
  prefix_length tinyint(3) unsigned DEFAULT '0' NOT NULL,
  maxbonus int(10) DEFAULT '0' NOT NULL,
  maxpublic int(10) DEFAULT '-1',
  maxdenials int(10) unsigned DEFAULT '0' NOT NULL,
  deny tinyint(3) unsigned DEFAULT '0' NOT NULL,
  trustpass tinyint(3) unsigned DEFAULT '0' NOT NULL,
  threshold tinyint(3) unsigned DEFAULT '4' NOT NULL, 
  maxconn tinyint(3)   unsigned DEFAULT '6' NOT NULL,
  comment longblob,
  PRIMARY KEY (prefix,prefix_length)
);

#
# Dumping data for table 'acl'
#

INSERT INTO acl VALUES (0,0,2000,-1,10,0,0,4,6,'Default access');


DROP TABLE IF EXISTS access;

#
# Access table for access control persistence
#
CREATE TABLE access (
  prefix          INT(10)    UNSIGNED DEFAULT '0' NOT NULL,
  prefix_length   TINYINT(3) UNSIGNED DEFAULT '0' NOT NULL,
  connections     INT(4)     UNSIGNED DEFAULT '0' NOT NULL,
  addr_passes     TINYINT(3) UNSIGNED DEFAULT '0' NOT NULL,
  denials         INT(4)     UNSIGNED DEFAULT '0' NOT NULL,
  queries         INT(4)     UNSIGNED DEFAULT '0' NOT NULL,
  referrals       INT(4)     UNSIGNED DEFAULT '0' NOT NULL,
  public_objects  INT(6)     UNSIGNED DEFAULT '0' NOT NULL,
  private_objects INT(6)     UNSIGNED DEFAULT '0' NOT NULL,
  public_bonus    REAL                DEFAULT '0' NOT NULL,
  private_bonus   REAL                DEFAULT '0' NOT NULL,
  timestamp       BIGINT     UNSIGNED NOT NULL,

  PRIMARY KEY     (prefix, prefix_length)
);

CREATE INDEX access_timestamp ON access(timestamp);

