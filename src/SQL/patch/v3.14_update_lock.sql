CREATE TABLE `update_lock` (
  `global_lock` int(11) NOT NULL,
  PRIMARY KEY (`global_lock`)
);

INSERT INTO update_lock VALUES (0);

