ALTER TABLE inaddr_arpa ADD `begin_in` int(10) unsigned NOT NULL default '0', ADD `end_in` int(10) unsigned NOT NULL default '0';
UPDATE inaddr_arpa SET begin_in = prefix & ~((1 << 32-prefix_length) - 1), end_in = prefix | ((1 << 32-prefix_length) - 1);
