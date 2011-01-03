INSERT INTO aaa VALUES (0, 0, 'DB-TEST', 1, 1, 'Default IPv4 access');
INSERT INTO aaa VALUES (0, 0, 'RIPE', 1, 1, 'Default IPv4 access');

INSERT INTO aaa6 VALUES (0, 0, 0, 0, 0, 'DB-TEST', 1, 1, 'Default IPv6 access');
INSERT INTO aaa6 VALUES (0, 0, 0, 0, 0, 'RIPE', 1, 1, 'Default IPv6 access');

INSERT INTO acl VALUES (0,0,2000,-1,10,0,0,4,6,'Default IPv4 access');

INSERT INTO acl6 VALUES (0,0,0,0,0,2000,-1,10,0,0,4,6,'Default IPv6 access');
