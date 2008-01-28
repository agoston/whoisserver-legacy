DELETE FROM aaa;
INSERT INTO aaa VALUES (0, 0, 'DB-TEST', 1, 1, 'Default IPv4 access');
INSERT INTO aaa VALUES (0, 0, 'RIPE', 1, 1, 'Default IPv4 access');

DELETE FROM aaa6;
INSERT INTO aaa6 VALUES (0, 0, 0, 0, 0, 'DB-TEST', 1, 1, 'Default IPv6 access');
INSERT INTO aaa6 VALUES (0, 0, 0, 0, 0, 'RIPE', 1, 1, 'Default IPv6 access');

DELETE FROM access;

DELETE FROM access6;

DELETE FROM acl;
INSERT INTO acl VALUES (0,0,2000,-1,10,0,0,4,6,'Default IPv4 access');

DELETE FROM acl6;
INSERT INTO acl6 VALUES (0,0,0,0,0,2000,-1,10,0,0,4,6,'Default IPv6 access');
