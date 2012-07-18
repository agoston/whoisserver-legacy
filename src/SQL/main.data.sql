INSERT mntner SET object_id=0, mntner='ANY', dummy=1;

INSERT INTO object_order VALUES (22,10);  # inet-reg
INSERT INTO object_order VALUES (6,11);   # inetnum
INSERT INTO object_order VALUES (23,20);  # inet6-reg
INSERT INTO object_order VALUES (5,21);   # inet6num
INSERT INTO object_order VALUES (0,30);   # as-block
INSERT INTO object_order VALUES (24,31);  # as-reg
INSERT INTO object_order VALUES (2,32);   # aut-num
INSERT INTO object_order VALUES (1,33);   # as-set
INSERT INTO object_order VALUES (12,40);  # route
INSERT INTO object_order VALUES (19,41);  # route6
INSERT INTO object_order VALUES (13,42);  # route-set
INSERT INTO object_order VALUES (4,50);   # inet-rtr
INSERT INTO object_order VALUES (14,60);  # filter-set
INSERT INTO object_order VALUES (15,61);  # peering-set
INSERT INTO object_order VALUES (16,62);  # rtr-set
INSERT INTO object_order VALUES (3,70);   # domain
INSERT INTO object_order VALUES (8,80);   # limerick
INSERT INTO object_order VALUES (21,81);  # poetic-form
INSERT INTO object_order VALUES (20,82);  # poem
INSERT INTO object_order VALUES (9,90);   # mntner
INSERT INTO object_order VALUES (17,91);  # irt
INSERT INTO object_order VALUES (7,100);  # key-cert
INSERT INTO object_order VALUES (25,110); # org-reg
INSERT INTO object_order VALUES (18,111); # organisation
INSERT INTO object_order VALUES (11,120); # role
INSERT INTO object_order VALUES (10,121); # person

INSERT INTO update_lock VALUES (0);
