DELETE FROM person_role;
DELETE FROM mntner;
DELETE FROM key_cert;
DELETE FROM aut_num;
DELETE FROM route;
DELETE FROM route6;
DELETE FROM as_set;
DELETE FROM route_set;
DELETE FROM inet_rtr;
DELETE FROM rtr_set;
DELETE FROM filter_set;
DELETE FROM peering_set;
DELETE FROM as_block;
DELETE FROM inet6num;
DELETE FROM inetnum;
DELETE FROM domain;
DELETE FROM limerick;
DELETE FROM irt;
DELETE FROM last;
DELETE FROM history;
DELETE FROM mbrs_by_ref;
DELETE FROM member_of;
DELETE FROM admin_c;
DELETE FROM tech_c;
DELETE FROM zone_c;
DELETE FROM mnt_by;
DELETE FROM mnt_irt;
DELETE FROM mnt_lower;
DELETE FROM mnt_domains;
DELETE FROM mnt_routes;
DELETE FROM mnt_routes6;
DELETE FROM author;
DELETE FROM referral_by;
DELETE FROM names;
DELETE FROM auth;
DELETE FROM auth_override;
DELETE FROM ifaddr;
DELETE FROM interface;
DELETE FROM rev_srv;
DELETE FROM refer;
DELETE FROM nserver;
DELETE FROM sub_dom;
DELETE FROM e_mail;
DELETE FROM notify;
DELETE FROM fingerpr;
DELETE FROM mnt_nfy;
DELETE FROM irt_nfy;
DELETE FROM upd_to;
DELETE FROM inaddr_arpa;
DELETE FROM ip6int;
DELETE FROM serials;
DELETE FROM nic_hdl;
DELETE FROM mnt_ref;
DELETE FROM organisation;
DELETE FROM ref_nfy;
DELETE FROM org_name;
DELETE FROM organisation_id;
DELETE FROM org;
DELETE FROM object_order;
DELETE FROM failed_transaction;
DELETE FROM transaction_rec;
DELETE FROM dummy_rec;
DELETE FROM tid;
DELETE FROM x509;
DELETE FROM poem;
DELETE FROM poetic_form;
DELETE FROM form;
DELETE FROM abuse_mailbox;
DELETE FROM ds_rdata;



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
INSERT INTO object_order VALUES (18,48);  #organisation
INSERT INTO object_order VALUES (20, 37); #poem
INSERT INTO object_order VALUES (21, 36); #poetic-form
