SUBDIRS = src
AUTOMAKE_OPTIONS = subdir-objects
PROVE = `dirname $(PERL5)`/prove
EXTRA_DIST = README \
	     README.MODULES \
	     README.freebsd \
	     README.linux \
	     README.solaris \
	     RELEASE-NOTES \
	     UPDATE-DB-3.2.0 \
	     UPDATE-DB-3.3.0 \
             build.sh \
	     conf/SAMPLE.CURRENTSERIAL \
	     conf/ack_template conf/ack_text \
	     conf/delcheck-erx-ipv4-16 \
	     conf/delcheck-ipv4-16 \
	     conf/delcheck-ipv4-24 \
	     conf/delcheck-ipv6-32 \
	     conf/delcheck-ipv6-36 \
	     conf/delcheck-ipv6-48 \
	     conf/delcheck-template.xml \
	     conf/ack_template.in \
	     conf/ack_text.in \
	     conf/forw_create_template.in \
	     conf/frwd_template.in \
	     conf/frwd_text.in \
	     conf/notif_template.in \
	     conf/notif_text.in \
	     conf/rip.config.sample.in \
	     conf/sample.db \
	     conf/SAMPLE.CURRENTSERIAL.in \
	     conf/sources.config.sample.in \
	     doc/WHOIS-HELP.in \
	     doc/LOOKUP-HELP.in \
             doc/DIFF-320-330.sql \
             doc/mysql-fine-tuning.txt \
	     src/utils/wrapper.in \
	     src/utils/make_mysql_user.in \
	     src/utils/remadmin.pl.in \
	     src/utils/ripe2rpsl.in \
	     src/utils/enableupdates.in \
	     src/utils/extract-keycerts-from-text-dump.pl.in \
	     src/utils/extract-keycerts-sql.pl.in \
	     src/utils/ip6arpa-fix.pl.in \
	     src/utils/radb_filter.pl.in \
	     src/utils/runqueue.in \
	     src/utils/resync-gnupg-ring-from-text-dump.sh.in \
	     src/utils/resync-gnupg-ring-from-sql.sh.in \
	     src/utils/restore_dbase.sh.in \
	     src/utils/ftp_export.sh.in \
	     src/utils/make_db.in \
	     src/utils/load_all.sh.in \
	     src/utils/whoisd.server.in \
	     src/utils/whoisd_start.in \
	     src/SQL/aux.index.2.in \
	     src/SQL/create.RIPADMIN.sql.in \
	     src/SQL/create.first-stage.in \
	     src/SQL/create.irt-tables.sql.patch.in \
	     src/SQL/create.last-history-indexes.sql.patch.in \
	     src/SQL/create.second-stage.in \
	     src/SQL/create.tables.sql.in \
	     src/SQL/create.tables.sql.patch.in \
	     src/SQL/create.third-stage.in \
	     src/SQL/fingerpr_x509.sql.patch.in \
	     src/SQL/leaf.index.2.in \
	     src/SQL/main.index.1.in \
             src/defs/AttributeDef.java \
             src/defs/AttributeSyntax.java \
             src/defs/ClassDef.java \
             src/defs/Defs.java \
             src/defs/Query.java \
             src/defs/RPSLSyntax.java \
             src/defs/ClassDefDOLLARDOLLARClassDiagram.class \
             src/defs/AttributeDef.class \
             src/defs/AttributeSyntax.class \
             src/defs/Defs.class \
             src/defs/Query.class \
             src/defs/RPSLSyntax.class \
	     src/defs/classes.xml \
	     src/defs/syntax.xml \
	     src/defs/attributes.xml \
	     src/defs/attributes.dtd \
	     src/defs/classes.dtd \
	     src/defs/syntax.dtd \
	     src/defs/Makefile.syntax \
	     src/defs/makedefsall.in \
             src/defs/HOWTO \
             src/defs/README \
             src/defs/alltodo \
             src/defs/variants/Makefile.syntax \
             src/defs/variants/RIPE/attributes.xml \
             src/defs/variants/RIPE/classes.xml \
             src/defs/variants/RIPE/syntax.xml \
             src/defs/xalan/README \
             src/defs/xalan/License \
             src/defs/xalan/xerces.jar \
             src/defs/Makefile.syntax \
             src/defs/attribute_tab.h \
             src/defs/attributes.dtd \
             src/defs/attributes.xml \
             src/defs/class_tab.h \
             src/defs/classes.dtd \
             src/defs/classes.xml \
             src/defs/makedefsall.in \
             src/defs/syntax.dtd \
             src/defs/AttributeSyntax.class \
             src/defs/RPSLSyntax.class \
             src/defs/ClassDef.class \
             src/defs/AttributeDef.class \
             src/defs/Query.class \
             src/defs/Defs.class \
             src/defs/syntax_tab.h \
             src/defs/makedefsall \
             src/defs/syntax.xml \
             src/include/rip.h \
             src/include/stubs.h \
             src/include/comparisons.h \
             src/include/globaldefs.h \
             src/include/QI_queries.def \
             src/include/UD_queries.def \
             src/include/DF_attribute_aliases.def \
             src/include/DF_attribute_aliases_map.def \
             src/include/DF_attribute_codes.def \
             src/include/DF_attribute_enum.def \
             src/include/DF_attribute_inv_attr_mask.def \
             src/include/DF_attribute_names.def \
             src/include/DF_class_aliases.def \
             src/include/DF_class_aliases_map.def \
             src/include/DF_class_codes.def \
             src/include/DF_class_dbase_code_map.def \
             src/include/DF_class_enum.def \
             src/include/DF_class_mask.def \
             src/include/DF_class_names.def \
             src/include/DF_class_templates.def \
             src/include/DF_class_templates_v.def \
             src/include/Diagrams.def \
             src/include/DF_radix_load.def \
             src/include/UD_comrol.def \
             src/include/NAME \
             src/utils/noperson.sed \
             src/utils/md5_pw.c \
             src/utils/README.ip6arpa \
             src/MODULES.txt \
             src/modules/rpsl/manual.txt \
             src/modules/rpsl/aggr_bndry.l \
             src/modules/rpsl/aggr_bndry.y \
             src/modules/rpsl/aggr_mtd.l \
             src/modules/rpsl/aggr_mtd.y \
             src/modules/rpsl/components.l \
             src/modules/rpsl/components.y \
             src/modules/rpsl/components_r6.l \
             src/modules/rpsl/components_r6.y \
             src/modules/rpsl/default.l \
             src/modules/rpsl/default.y \
             src/modules/rpsl/export.l \
             src/modules/rpsl/export.y \
             src/modules/rpsl/filter.l \
             src/modules/rpsl/filter.y \
             src/modules/rpsl/ifaddr.l \
             src/modules/rpsl/ifaddr.y \
             src/modules/rpsl/import.l \
             src/modules/rpsl/import.y \
             src/modules/rpsl/inet6num.l \
             src/modules/rpsl/inet6num.y \
             src/modules/rpsl/inject.l \
             src/modules/rpsl/inject.y \
             src/modules/rpsl/inject_r6.l \
             src/modules/rpsl/inject_r6.y \
             src/modules/rpsl/interface.l \
             src/modules/rpsl/interface.y \
             src/modules/rpsl/members_is.l \
             src/modules/rpsl/members_is.y \
             src/modules/rpsl/members_rs.l \
             src/modules/rpsl/members_rs.y \
             src/modules/rpsl/mnt_routes.l \
             src/modules/rpsl/mnt_routes.y \
             src/modules/rpsl/mnt_routes6.l \
             src/modules/rpsl/mnt_routes6.y \
             src/modules/rpsl/mnt_routes_an.l \
             src/modules/rpsl/mnt_routes_an.y \
             src/modules/rpsl/mp_default.l \
             src/modules/rpsl/mp_default.y \
             src/modules/rpsl/mp_export.l \
             src/modules/rpsl/mp_export.y \
             src/modules/rpsl/mp_filter.l \
             src/modules/rpsl/mp_filter.y \
             src/modules/rpsl/mp_import.l \
             src/modules/rpsl/mp_import.y \
             src/modules/rpsl/mp_members_is.l \
             src/modules/rpsl/mp_members_is.y \
             src/modules/rpsl/mp_members_rs.l \
             src/modules/rpsl/mp_members_rs.y \
             src/modules/rpsl/mp_peer.l \
             src/modules/rpsl/mp_peer.y \
             src/modules/rpsl/mp_peering.l \
             src/modules/rpsl/mp_peering.y \
             src/modules/rpsl/peer.l \
             src/modules/rpsl/peer.y \
             src/modules/rpsl/peering.l \
             src/modules/rpsl/peering.y \
             src/modules/rpsl/refer.l \
             src/modules/rpsl/refer.y \
             src/modules/rpsl/v6_filter.l \
             src/modules/rpsl/v6_filter.y \
             src/modules/rpsl/attribute.h \
             src/modules/rpsl/attribute.c \
             src/modules/rpsl/class.c \
             src/modules/rpsl/rpsl_test.c \
             src/modules/rpsl/syntax.c \
             src/modules/rpsl/syntax_api.c \
             src/modules/rpsl/test_syntax.c \
             src/modules/rpsl/class.h \
             src/modules/rpsl/manual.txt \
             src/modules/rpsl/syntax.h \
             src/modules/rpsl/syntax_api.h \
             src/modules/rpsl/aggr_bndry.lex.c \
             src/modules/rpsl/aggr_bndry.tab.c \
             src/modules/rpsl/aggr_mtd.lex.c \
             src/modules/rpsl/aggr_mtd.tab.c \
             src/modules/rpsl/attribute.c \
             src/modules/rpsl/class.c \
             src/modules/rpsl/components.lex.c \
             src/modules/rpsl/components_r6.lex.c \
             src/modules/rpsl/components_r6.tab.c \
             src/modules/rpsl/components.tab.c \
             src/modules/rpsl/default.lex.c \
             src/modules/rpsl/default.tab.c \
             src/modules/rpsl/export.lex.c \
             src/modules/rpsl/export.tab.c \
             src/modules/rpsl/filter.lex.c \
             src/modules/rpsl/filter.tab.c \
             src/modules/rpsl/ifaddr.lex.c \
             src/modules/rpsl/ifaddr.tab.c \
             src/modules/rpsl/import.lex.c \
             src/modules/rpsl/import.tab.c \
             src/modules/rpsl/inet6num.lex.c \
             src/modules/rpsl/inet6num.tab.c \
             src/modules/rpsl/inject.lex.c \
             src/modules/rpsl/inject_r6.lex.c \
             src/modules/rpsl/inject_r6.tab.c \
             src/modules/rpsl/inject.tab.c \
             src/modules/rpsl/interface.lex.c \
             src/modules/rpsl/interface.tab.c \
             src/modules/rpsl/members_is.lex.c \
             src/modules/rpsl/members_is.tab.c \
             src/modules/rpsl/members_rs.lex.c \
             src/modules/rpsl/members_rs.tab.c \
             src/modules/rpsl/mnt_routes6.lex.c \
             src/modules/rpsl/mnt_routes6.tab.c \
             src/modules/rpsl/mnt_routes_an.lex.c \
             src/modules/rpsl/mnt_routes_an.tab.c \
             src/modules/rpsl/mnt_routes.lex.c \
             src/modules/rpsl/mnt_routes.tab.c \
             src/modules/rpsl/mp_default.lex.c \
             src/modules/rpsl/mp_default.tab.c \
             src/modules/rpsl/mp_export.lex.c \
             src/modules/rpsl/mp_export.tab.c \
             src/modules/rpsl/mp_filter.lex.c \
             src/modules/rpsl/mp_filter.tab.c \
             src/modules/rpsl/mp_import.lex.c \
             src/modules/rpsl/mp_import.tab.c \
             src/modules/rpsl/mp_members_is.lex.c \
             src/modules/rpsl/mp_members_is.tab.c \
             src/modules/rpsl/mp_members_rs.lex.c \
             src/modules/rpsl/mp_members_rs.tab.c \
             src/modules/rpsl/mp_peering.lex.c \
             src/modules/rpsl/mp_peering.tab.c \
             src/modules/rpsl/mp_peer.lex.c \
             src/modules/rpsl/mp_peer.tab.c \
             src/modules/rpsl/peering.lex.c \
             src/modules/rpsl/peering.tab.c \
             src/modules/rpsl/peer.lex.c \
             src/modules/rpsl/peer.tab.c \
             src/modules/rpsl/refer.lex.c \
             src/modules/rpsl/refer.tab.c \
             src/modules/rpsl/rpsl_test.c \
             src/modules/rpsl/syntax_api.c \
             src/modules/rpsl/syntax.c \
             src/modules/rpsl/test_syntax.c \
             src/modules/rpsl/v6_filter.lex.c \
             src/modules/rpsl/v6_filter.tab.c \
             src/modules/rpsl/aggr_bndry.tab.h \
             src/modules/rpsl/aggr_mtd.tab.h \
             src/modules/rpsl/attribute.h \
             src/modules/rpsl/class.h \
             src/modules/rpsl/components_r6.tab.h \
             src/modules/rpsl/components.tab.h \
             src/modules/rpsl/default.tab.h \
             src/modules/rpsl/export.tab.h \
             src/modules/rpsl/filter.tab.h \
             src/modules/rpsl/ifaddr.tab.h \
             src/modules/rpsl/import.tab.h \
             src/modules/rpsl/inet6num.tab.h \
             src/modules/rpsl/inject_r6.tab.h \
             src/modules/rpsl/inject.tab.h \
             src/modules/rpsl/interface.tab.h \
             src/modules/rpsl/members_is.tab.h \
             src/modules/rpsl/members_rs.tab.h \
             src/modules/rpsl/mnt_routes6.tab.h \
             src/modules/rpsl/mnt_routes_an.tab.h \
             src/modules/rpsl/mnt_routes.tab.h \
             src/modules/rpsl/mp_default.tab.h \
             src/modules/rpsl/mp_export.tab.h \
             src/modules/rpsl/mp_filter.tab.h \
             src/modules/rpsl/mp_import.tab.h \
             src/modules/rpsl/mp_members_is.tab.h \
             src/modules/rpsl/mp_members_rs.tab.h \
             src/modules/rpsl/mp_peering.tab.h \
             src/modules/rpsl/mp_peer.tab.h \
             src/modules/rpsl/peering.tab.h \
             src/modules/rpsl/peer.tab.h \
             src/modules/rpsl/refer.tab.h \
             src/modules/rpsl/syntax_api.h \
             src/modules/rpsl/syntax.h \
             src/modules/rpsl/v6_filter.tab.h \
             src/modules/aa/aa.h \
             src/modules/ac/access_control.h \
             src/modules/au/au.h \
             src/modules/au/au_plugins.h \
             src/modules/au/au_rpsl.h \
             src/modules/au/au_util.h \
             src/modules/au/crypt-whois.h \
             src/modules/au/TODO \
             src/modules/au/auth_spec.txt \
             src/modules/ca/ca_adminAttribs.h \
             src/modules/ca/ca_configFns.h \
             src/modules/ca/ca_defs.h \
             src/modules/ca/ca_dictionary.h \
             src/modules/ca/ca_libs.h \
             src/modules/ca/ca_macros.h \
             src/modules/ca/ca_srcAttribs.h \
             src/modules/ca/README \
             src/modules/ca/RELEASE_NOTES.v3 \
             src/modules/ca/RELEASE_NOTES.v3.1 \
             src/modules/ca/configFileDoc.txt \
             src/modules/ca/dictGen.m4 \
             src/modules/co/constants.h \
             src/modules/cr/cr.h \
             src/modules/df/defs.h \
             src/modules/ep/TODO \
             src/modules/ep/ep.h \
             src/modules/ep/ep_internal.h \
             src/modules/ep/mm.h \
             src/modules/ep/mm_internal.h \
             src/modules/ip/iproutines.h \
             src/modules/km/km.h \
             src/modules/km/BUGS \
             src/modules/km/km_internal.h \
             src/modules/lg/lg.h \
             src/modules/lu/lu.h \
             src/modules/lu/lu_api.txt \
             src/modules/lu/lu_whois.h \
             src/modules/ma/bitmask.h \
             src/modules/nh/nh.h \
             src/modules/ns/ns_rir.h \
             src/modules/ns/ns_util.h \
             src/modules/ns/ns_xml.h \
             src/modules/nt/notification.h \
             src/modules/pc/pc_commands.h \
             src/modules/pc/protocol_config.h \
             src/modules/pg/pg.h \
             src/modules/pm/protocol_mirror.h \
             src/modules/pr/properties.h \
             src/modules/pw/protocol_whois.h \
             src/modules/pl/protocol_lookup.h \
             src/modules/qc/mg_getopt.h \
             src/modules/qc/query_command.h \
             src/modules/qi/query_instructions.h \
             src/modules/rp/rp.h \
             src/modules/rt/rt_dbupdate.h \
             src/modules/rt/rt.h \
             src/modules/rt/rt_internal.h \
             src/modules/rx/rxroutines.h \
             src/modules/sk/sk.h \
             src/modules/sq/mysql_driver.h \
             src/modules/sv/server.h \
             src/modules/ta/ta.h \
             src/modules/th/thread.h \
             src/modules/ud/ud.h \
             src/modules/ud/ud_int.h \
             src/modules/ud/ud_tr.h \
             src/modules/up/dbupdate.h \
             src/modules/up/up_auto_nic.h \
             src/modules/up/up_pre_process.h \
             src/modules/up/up_util.h \
             src/modules/up/TODO \
             src/modules/ut/fdwrap.h \
             src/modules/ut/memwrap.h \
             src/modules/ut/numconv.h \
             src/modules/ut/timediff.h \
             src/modules/ut/ut.h \
             src/modules/ut/ut_string.h \
             src/modules/wh/wh_queries.h \
             src/modules/wk/which_keytypes.h \
             src/history/aconf.h \
             src/history/dbsupport.h \
             src/history/miniconf.h \
             src/opl-perl/Makefile.PL.in

#MOSTLYCLEANFILES = ./src/modules/rpsl/*.o \
#		   ./src/modules/rpsl/*.lex.c \
#		   ./src/modules/rpsl/*.tab.c \
#		   ./src/modules/rpsl/syntax \
#		   ./conf/sample.db.gz \
#		   ./src/defs/*.class \
#		   ./src/defs/Makefile.syntax \
#		   ./src/defs/attribute_tab.h \
#		   ./src/defs/syntax_tab.h \
#		   ./src/defs/class_tab.h \
#		   ./src/defs/alltodo \
#		   ./src/include/*.def \
#		   ./src/librpsl.a

#DISTCLEANFILES = ./src/modules/rpsl/*.y \
#                 ./src/modules/rpsl/*.l \
#                 ./src/modules/rpsl/attribute.c \
#                 ./src/modules/rpsl/class.c \
#                 ./src/modules/rpsl/rpsl_test.c \
#                 ./src/modules/rpsl/syntax.c \
#                 ./src/modules/rpsl/test_syntax.c \
#                 ./src/defs/attributes.xml \
#                 ./src/defs/classes.xml \
#                 ./src/defs/syntax.xml

uninstall-local:
	rm -f $(DESTDIR)/${bindir}/SQL/aux.index.2
	rm -f $(DESTDIR)/${bindir}/SQL/create.RIPADMIN.sql
	rm -f $(DESTDIR)/${bindir}/SQL/create.tables.sql
	rm -f $(DESTDIR)/${bindir}/SQL/leaf.index.2
	rm -f $(DESTDIR)/${bindir}/SQL/main.index.1
	rm -f $(DESTDIR)/${bindir}/SQL/create.first-stage
	rm -f $(DESTDIR)/${bindir}/SQL/create.second-stage
	rm -f $(DESTDIR)/${bindir}/SQL/create.third-stage
	rm -f $(DESTDIR)/${bindir}/remadmin.pl
	rm -f $(DESTDIR)/${bindir}/make_mysql_user
	rm -f $(DESTDIR)/${bindir}/ripe2rpsl
	rm -f $(DESTDIR)/${bindir}/make_db
	rm -f $(DESTDIR)/${bindir}/load_all.sh
	rm -f $(DESTDIR)/${bindir}/whoisd.server
	rm -f $(DESTDIR)/${bindir}/whoisd_start
	rm -f $(DESTDIR)/${confdir}/rip.config.sample
	rm -f $(DESTDIR)/${confdir}/sources.config.sample
	rm -f $(DESTDIR)/${confdir}/ack_template
	rm -f $(DESTDIR)/${confdir}/ack_text
	rm -f $(DESTDIR)/${confdir}/forw_create_template
	rm -f $(DESTDIR)/${confdir}/frwd_template
	rm -f $(DESTDIR)/${confdir}/frwd_text
	rm -f $(DESTDIR)/${confdir}/notif_template
	rm -f $(DESTDIR)/${confdir}/notif_text
	rm -f $(DESTDIR)/${docdir}/WHOIS-HELP
	rm -f $(DESTDIR)/${docdir}/LOOKUP-HELP
	rm -f $(DESTDIR)/${tmpdir}/load/SAMPLE/sample.db.gz
	rm -f $(DESTDIR)/${tmpdir}/load/SAMPLE/SAMPLE.CURRENTSERIAL

distclean-local:
	rm -f src/modules/rpsl/*.o

cadict:
	$(M4) -d src/modules/ca/dictGen.m4 > src/modules/ca/ca_dictionary.h
