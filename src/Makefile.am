INCLUDES =  \
  $(GLIB_CFLAGS) \
  $(MYSQL_CFLAGS) \
  $(XSLT_CFLAGS) \
  $(XML_CFLAGS) \
  $(CFLAGS) \
  $(RDNSCFLAGS) \
  -I$(cclientinc) \
  -Iinclude -Imodules/rpsl

LDRIPNOMYSQL =  $(top_srcdir)/src/librip.a $(GLIB_LIBS)
LDRIP = $(LDRIPNOMYSQL) $(MYSQL_LIBS)
LDRIPFULL = $(top_srcdir)/src/librip.a $(cclientlib)/c-client.a \
	    $(GLIB_LIBS) $(MYSQL_LIBS) \
	    $(XML_LIBS) $(XSLT_LIBS) \
            $(RDNSLDFLAGS)
LDHISTORY = libsq.a liblg.a libut.a \
	    $(GLIB_LIBS) $(MYSQL_LIBS)

OPLLDLOADLIBS = $(GLIB_LIBS) -L../opl -lrpsl $(GLIB_LIBS)
OPLMAKE = $(MAKE) LDDLFLAGS=" -shared $(OPLLDLOADLIBS)" LDFLAGS="$(OPLLDLOADLIBS)" LDLOADLIBS="$(OPLLDLOADLIBS)"

EXTRA_DIST = include/rip.h \
             include/stubs.h \
             include/comparisons.h \
             include/globaldefs.h \
             include/QI_queries.def \
             include/UD_queries.def \
             include/DF_attribute_aliases.def \
             include/DF_attribute_aliases_map.def \
             include/DF_attribute_codes.def \
             include/DF_attribute_enum.def \
             include/DF_attribute_inv_attr_mask.def \
             include/DF_attribute_names.def \
             include/DF_class_aliases.def \
             include/DF_class_aliases_map.def \
             include/DF_class_codes.def \
             include/DF_class_dbase_code_map.def \
             include/DF_class_enum.def \
             include/DF_class_mask.def \
             include/DF_class_names.def \
             include/DF_class_templates.def \
             include/DF_class_templates_v.def \
             include/Diagrams.def \
             include/DF_radix_load.def \
             include/UD_comrol.def \
             include/NAME \
             utils/noperson.sed \
             utils/md5_pw.c \
             utils/README.ip6arpa \
             modules/MODULES \
             modules/rpsl/manual.txt \
             modules/rpsl/aggr_bndry.l \
             modules/rpsl/aggr_bndry.y \
             modules/rpsl/aggr_mtd.l \
             modules/rpsl/aggr_mtd.y \
             modules/rpsl/components.l \
             modules/rpsl/components.y \
             modules/rpsl/components_r6.l \
             modules/rpsl/components_r6.y \
             modules/rpsl/default.l \
             modules/rpsl/default.y \
             modules/rpsl/export.l \
             modules/rpsl/export.y \
             modules/rpsl/filter.l \
             modules/rpsl/filter.y \
             modules/rpsl/ifaddr.l \
             modules/rpsl/ifaddr.y \
             modules/rpsl/import.l \
             modules/rpsl/import.y \
             modules/rpsl/inet6num.l \
             modules/rpsl/inet6num.y \
             modules/rpsl/inject.l \
             modules/rpsl/inject.y \
             modules/rpsl/inject_r6.l \
             modules/rpsl/inject_r6.y \
             modules/rpsl/interface.l \
             modules/rpsl/interface.y \
             modules/rpsl/members_is.l \
             modules/rpsl/members_is.y \
             modules/rpsl/members_rs.l \
             modules/rpsl/members_rs.y \
             modules/rpsl/mnt_routes.l \
             modules/rpsl/mnt_routes.y \
             modules/rpsl/mnt_routes6.l \
             modules/rpsl/mnt_routes6.y \
             modules/rpsl/mnt_routes_an.l \
             modules/rpsl/mnt_routes_an.y \
             modules/rpsl/mp_default.l \
             modules/rpsl/mp_default.y \
             modules/rpsl/mp_export.l \
             modules/rpsl/mp_export.y \
             modules/rpsl/mp_filter.l \
             modules/rpsl/mp_filter.y \
             modules/rpsl/mp_import.l \
             modules/rpsl/mp_import.y \
             modules/rpsl/mp_members_is.l \
             modules/rpsl/mp_members_is.y \
             modules/rpsl/mp_members_rs.l \
             modules/rpsl/mp_members_rs.y \
             modules/rpsl/mp_peer.l \
             modules/rpsl/mp_peer.y \
             modules/rpsl/mp_peering.l \
             modules/rpsl/mp_peering.y \
             modules/rpsl/peer.l \
             modules/rpsl/peer.y \
             modules/rpsl/peering.l \
             modules/rpsl/peering.y \
             modules/rpsl/refer.l \
             modules/rpsl/refer.y \
             modules/rpsl/v6_filter.l \
             modules/rpsl/v6_filter.y \
             modules/rpsl/attribute.c \
             modules/rpsl/attribute.h \
             modules/rpsl/class.c \
             modules/rpsl/class.h \
             modules/rpsl/manual.txt \
             modules/rpsl/rpsl_test.c \
             modules/rpsl/syntax.c \
             modules/rpsl/syntax.h \
             modules/rpsl/syntax_api.c \
             modules/rpsl/syntax_api.h \
             modules/rpsl/test_syntax.c \
             modules/aa/aa.h \
             modules/ac/access_control.h \
             modules/au/au.h \
             modules/au/au_plugins.h \
             modules/au/au_rpsl.h \
             modules/au/au_util.h \
             modules/au/crypt-whois.h \
             modules/au/TODO \
             modules/au/auth_spec.txt \
             modules/ca/ca_adminAttribs.h \
             modules/ca/ca_configFns.h \
             modules/ca/ca_defs.h \
             modules/ca/ca_dictionary.h \
             modules/ca/ca_libs.h \
             modules/ca/ca_macros.h \
             modules/ca/ca_srcAttribs.h \
             modules/ca/README \
             modules/ca/RELEASE_NOTES.v3 \
             modules/ca/RELEASE_NOTES.v3.1 \
             modules/ca/configFileDoc.txt \
             modules/ca/dictGen.m4 \
             modules/co/constants.h \
             modules/cr/cr.h \
             modules/df/defs.h \
             modules/ep/TODO \
             modules/ep/ep.h \
             modules/ep/ep_internal.h \
             modules/ep/mm.h \
             modules/ep/mm_internal.h \
             modules/ip/inet6def.h \
             modules/ip/iproutines.h \
             modules/km/km.h \
             modules/km/BUGS \
             modules/km/km_internal.h \
             modules/lg/lg.h \
             modules/lu/lu.h \
             modules/lu/lu_api.txt \
             modules/lu/lu_whois.h \
             modules/ma/bitmask.h \
             modules/nh/nh.h \
             modules/ns/ns_perl.h \
             modules/ns/ns_rir.h \
             modules/ns/ns_util.h \
             modules/ns/ns_xml.h \
             modules/nt/notification.h \
             modules/nt/lu_api.txt \
             modules/pc/pc_commands.h \
             modules/pc/protocol_config.h \
             modules/pg/pg.h \
             modules/pm/protocol_mirror.h \
             modules/pr/properties.h \
             modules/pw/protocol_whois.h \
             modules/qc/mg_getopt.h \
             modules/qc/query_command.h \
             modules/qi/query_instructions.h \
             modules/rp/rp.h \
             modules/rt/rt_dbupdate.h \
             modules/rt/rt.h \
             modules/rt/rt_internal.h \
             modules/rx/rxroutines.h \
             modules/sk/sk.h \
             modules/sq/mysql_driver.h \
             modules/sv/server.h \
             modules/ta/ta.h \
             modules/th/thread.h \
             modules/ud/ud.h \
             modules/ud/ud_int.h \
             modules/ud/ud_tr.h \
             modules/up/dbupdate.h \
             modules/up/up_auto_nic.h \
             modules/up/up_pre_process.h \
             modules/up/up_util.h \
             modules/up/TODO \
             modules/ut/fdwrap.h \
             modules/ut/memwrap.h \
             modules/ut/numconv.h \
             modules/ut/timediff.h \
             modules/ut/ut.h \
             modules/ut/ut_string.h \
             modules/wh/wh_queries.h \
             modules/wk/which_keytypes.h \
             history/aconf.h \
             history/dbsupport.h \
             history/miniconf.h

SUBDIRS = defs

if BUILDRDNS
LIBNS = libns.a
else
LIBNS =
endif

noinst_LIBRARIES = libaa.a libac.a libau.a libca.a libco.a libcr.a \
                   libdf.a libep.a libip.a libkm.a liblg.a liblu.a \
                   libma.a libnh.a $(LIBNS) libnt.a libpc.a libpg.a \
                   libpm.a libpr.a libpw.a libqc.a libqi.a librp.a \
                   librt.a librx.a libsk.a libsq.a libsv.a libta.a \
		   libth.a libud.a libup.a libut.a libwh.a libwk.a \
		   librip.a

.PHONY: librip.a opl

librip.a:
	$(AR) cru librip.a *.o modules/rpsl/*.o

opl: ut.o ut_string.o ip.o lg.o memwrap.o numconv.o syntax
	rm -f opl/librpsl.a ; $(AR) cru opl/librpsl.a modules/rpsl/*.o ut.o ut_string.o ip.o lg.o memwrap.o numconv.o

opl-perl: opl
	cd opl-perl; \
	$(PERL5) Makefile.PL; \
	$(OPLMAKE) ; \
	cd ..

opl-perl-install: opl-perl
	cd opl-perl; \
	$(OPLMAKE) install; \
	cd ..

opl-perl-dist: opl-perl
	cd opl-perl; \
	$(OPLMAKE) dist; \
	cd ..

syntax:
	cd defs ; make syntax ; cd ..

libaa_a_SOURCES = modules/aa/aa.c
libac_a_SOURCES = modules/ac/ac_persistence.c modules/ac/access_control.c
libau_a_SOURCES = modules/au/au_irt.c modules/au/au_main.c \
                  modules/au/au_organisation.c modules/au/au_plugins.c \
                  modules/au/au_ripe.c modules/au/au_rpsl.c \
                  modules/au/au_util.c modules/au/crypt-md5.c \
                  modules/au/misc.c modules/au/test_au_dbupdate.c
libca_a_SOURCES = modules/ca/ca_configFns.c modules/ca/ca_initFn.c \
                  modules/ca/ca_sanityCheck.c \
                  modules/ca/ca_sourceLoader.c modules/ca/ca_values.c
libco_a_SOURCES = modules/co/constants.c
libcr_a_SOURCES = modules/cr/cr.c
libdf_a_SOURCES = modules/df/defs.c
libep_a_SOURCES = modules/ep/ep.c modules/ep/ep_mail_driver.c \
                  modules/ep/ep_text_driver.c modules/ep/mm.c \
                  modules/ep/mm_hook.c modules/ep/x509.c
libip_a_SOURCES = modules/ip/inet_ntop.c modules/ip/inet_pton.c \
                  modules/ip/ip.c
libkm_a_SOURCES = modules/km/km.c modules/km/km_X509.c \
                  modules/km/km_pgp.c modules/km/lulu.c
liblg_a_SOURCES = modules/lg/lg.c modules/lg/lg_utils.c
liblu_a_SOURCES = modules/lu/lu.c modules/lu/lu_whois.c \
                  modules/lu/test_lu_whois.c
libma_a_SOURCES = modules/ma/bitmask.c
libnh_a_SOURCES = modules/nh/nh.c
libns_a_SOURCES = modules/ns/ns_auth.c modules/ns/ns_perl.c \
                  modules/ns/ns_rir.c modules/ns/ns_util.c \
                  modules/ns/ns_xml.c
libnt_a_SOURCES = modules/nt/notification.c
libpc_a_SOURCES = modules/pc/pc_commands.c modules/pc/protocol_config.c
libpg_a_SOURCES = modules/pg/pg.c
libpm_a_SOURCES = modules/pm/pm_serials.c modules/pm/protocol_mirror.c
libpr_a_SOURCES = modules/pr/properties.c
libpw_a_SOURCES = modules/pw/protocol_whois.c
libqc_a_SOURCES = modules/qc/mg_getopt.c modules/qc/query_command.c
libqi_a_SOURCES = modules/qi/query_instructions.c
librp_a_SOURCES = modules/rp/rp_convert.c modules/rp/rp_load.c \
                  modules/rp/rp_search.c modules/rp/rp_tree.c \
                  modules/rp/rp_update.c
librt_a_SOURCES = modules/rt/rt_core.c modules/rt/rt_dbupdate.c
librx_a_SOURCES = modules/rx/rx_node.c modules/rx/rx_print.c \
                  modules/rx/rx_search.c modules/rx/rx_tree.c
libsk_a_SOURCES = modules/sk/cd_socket.c modules/sk/cd_watchdog.c \
                  modules/sk/sk_socket.c
libsq_a_SOURCES = modules/sq/mysql_driver.c
libsv_a_SOURCES = modules/sv/server.c
libta_a_SOURCES = modules/ta/ta.c
libth_a_SOURCES = modules/th/thread.c
libud_a_SOURCES = modules/ud/ud_comrol.c modules/ud/ud_core.c \
                  modules/ud/ud_main.c modules/ud/ud_misc.c \
                  modules/ud/ud_process_stream.c modules/ud/ud_recover.c \
                  modules/ud/ud_rx.c modules/ud/ud_serial.c
libup_a_SOURCES = modules/up/up_auto_nic.c modules/up/up_pre_process.c \
                  modules/up/up_util.c
libut_a_SOURCES = modules/ut/fdwrap.c modules/ut/memwrap.c \
                  modules/ut/numconv.c modules/ut/timediff.c \
                  modules/ut/ut.c modules/ut/ut_string.c
libwh_a_SOURCES = modules/wh/wh_queries.c
libwk_a_SOURCES = modules/wk/which_keytypes.c

bin_PROGRAMS = dbupdate whois-server \
               loader getvar \
               history/archive history/surf history/recreate \
               history/cleanup history/backup \
               mr ripupdate \
               text_export

dbupdate_SOURCES = dbupdate_src/dbupdate.c
dbupdate_LDADD = $(LDRIPFULL)

whois_server_SOURCES = whois-server_src/whois-server.c
whois_server_LDADD = $(LDRIP)

loader_SOURCES = loader_src/loader.c
loader_LDADD = $(LDRIP)

getvar_SOURCES = getvar_src/getvar.c
getvar_LDADD = $(LDRIPNOMYSQL)

history_archive_SOURCES = history/miniconf.c history/aconf.c \
                          history/archive.c history/dbsupport.c
history_archive_LDADD = $(LDHISTORY)

history_surf_SOURCES = history/surf.c history/dbsupport.c \
                       history/miniconf.c history/aconf.c
history_surf_LDADD = $(LDHISTORY)

history_recreate_SOURCES = history/recreate.c history/dbsupport.c \
                           history/miniconf.c history/aconf.c
history_recreate_LDADD = $(LDRIPFULL)

history_cleanup_SOURCES = history/cleanup.c history/dbsupport.c \
                          history/miniconf.c history/aconf.c
history_cleanup_LDADD = $(LDHISTORY)

history_backup_SOURCES = history/backup.c history/dbsupport.c \
                         history/miniconf.c history/aconf.c
history_backup_LDADD = $(LDHISTORY)

ripupdate_SOURCES = ripupdate_src/ripupdate.c

mr_SOURCES = mr_src/mr.c
mr_LDADD = $(LDRIPNOMYSQL)

text_export_SOURCES = text_export_src/text_export.c
text_export_LDADD = $(LDRIP)
