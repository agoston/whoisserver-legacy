INCLUDES =  \
  $(GLIB_CFLAGS) \
  $(MYSQL_CFLAGS) \
  $(XSLT_CFLAGS) \
  $(XML_CFLAGS) \
  $(CFLAGS) \
  $(RDNSCFLAGS) \
  -I$(cclientinc) \
  -I@abs_top_srcdir@/src/modules/rpsl -I@abs_top_builddir@/src/include -I@abs_top_srcdir@/src/include

LDRIPNOMYSQL =  @abs_top_builddir@/src/librip.a $(GLIB_LIBS)
LDRIP = $(LDRIPNOMYSQL) $(MYSQL_LIBS)
LDRIPFULL = @abs_top_builddir@/src/librip.a $(cclientlib)/c-client.a \
	    $(GLIB_LIBS) $(MYSQL_LIBS) \
	    $(XSLT_LIBS) $(XML_LIBS) \
            $(RDNSLDFLAGS)
LDHISTORY = libsq.a liblg.a libut.a \
	    $(GLIB_LIBS) $(MYSQL_LIBS)

OPLLDLOADLIBS = $(GLIB_LIBS) -L../opl -lrpsl $(GLIB_LIBS)
OPLMAKE = $(MAKE) LDDLFLAGS=" -shared $(OPLLDLOADLIBS)" LDFLAGS="$(OPLLDLOADLIBS)" LDLOADLIBS="$(OPLLDLOADLIBS)"

SUBDIRS = defs

PROVE = `dirname $(PERL5)`/prove

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
	$(AR) cru @abs_top_builddir@/src/librip.a @abs_top_builddir@/src/*.o @abs_top_builddir@/src/modules/rpsl/*.o

opl: ut.o ut_string.o ip.o lg.o memwrap.o numconv.o thread.o ca_configFns.o  ca_initFn.o  ca_sanityCheck.o  ca_sourceLoader.o  ca_values.o syntax
	rm -f opl/librpsl.a ; $(AR) cru opl/librpsl.a modules/rpsl/*.o ut.o ut_string.o ip.o lg.o memwrap.o numconv.o thread.o ca_configFns.o  ca_initFn.o  ca_sanityCheck.o  ca_sourceLoader.o  ca_values.o

opl-install: opl
	${mkinstalldirs} $(libdir)
	${INSTALL} -m 0644 ./opl/librpsl.a ${DESTDIR}$(libdir)

opl-uninstall:
	rm -vf ${DESTDIR}$(libdir)/librpsl.a

opl-perl: opl
	${INSTALL} -m 0644 opl/librpsl.a opl-perl/librpsl_perl_dist.a
	cd opl-perl; \
	$(PERL5) Makefile.PL; \
	$(OPLMAKE) ; \
	cd ..

opl-perl-test: opl-perl opl-install
	cd opl-perl; \
	$(OPLMAKE) test; \
	cd ..

opl-perl-install: opl-perl opl-install
	cd opl-perl; \
	$(OPLMAKE) install; \
	cd ..

opl-perl-uninstall: opl-uninstall
	cd opl-perl; \
	$(OPLMAKE) uninstall; \
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
libip_a_SOURCES = modules/ip/ip.c
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
                  modules/rp/rp_update.c modules/rp/rp_interact.c
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
               mr ripupdate \
               text_export

dbupdate_SOURCES = dbupdate_src/dbupdate.c
dbupdate_LDADD = $(LDRIPFULL)

whois_server_SOURCES = whois-server_src/whois-server.c
whois_server_LDADD = $(LDRIP)

loader_SOURCES = loader_src/loader.c
loader_LDADD = $(LDRIP)

getvar_SOURCES = getvar_src/getvar.c
# FIXME: this should be LDRIPNOMYSQL, but we have compilation problems on the old whois boxes
#getvar_LDADD = $(LDRIPNOMYSQL)
getvar_LDADD = $(LDRIP)

#archive_SOURCES = history/miniconf.c history/aconf.c history/archive.c history/dbsupport.c
#archive_LDADD = $(LDHISTORY)

#surf_SOURCES = history/surf.c history/dbsupport.c history/miniconf.c history/aconf.c
#surf_LDADD = $(LDHISTORY)

# recreate_SOURCES = history/recreate.c history/dbsupport.c history/miniconf.c history/aconf.c
# recreate_LDADD = $(LDRIPFULL)

# cleanup_SOURCES = history/cleanup.c history/dbsupport.c history/miniconf.c history/aconf.c
# cleanup_LDADD = $(LDHISTORY)

# backup_SOURCES = history/backup.c history/dbsupport.c history/miniconf.c history/aconf.c
# backup_LDADD = $(LDHISTORY)

ripupdate_SOURCES = ripupdate_src/ripupdate.c

mr_SOURCES = mr_src/mr.c
# FIXME: this should be LDRIPNOMYSQL, but we have compilation problems on the old whois boxes
#mr_LDADD = $(LDRIPNOMYSQL)
mr_LDADD = $(LDRIP)

text_export_SOURCES = text_export_src/text_export.c
text_export_LDADD = $(LDRIP)

install-exec-hook:
	${mkinstalldirs} $(DESTDIR)/${topdir} ${DESTDIR}${bindir} \
	  $(DESTDIR)/${confdir} \
	  $(DESTDIR)/${docdir} \
	  $(DESTDIR)/${bindir}/SQL \
	  $(DESTDIR)/${vardir} ${DESTDIR}${tmpdir} \
	  $(DESTDIR)/${tmpdir}/load ${DESTDIR}${tmpdir}/load/SAMPLE \
	  $(DESTDIR)/${lockdir} ${DESTDIR}${logdir} \
	  $(DESTDIR)/${qrylog} ${DESTDIR}${dbuperr} \
	  $(DESTDIR)/${allriperr} ${DESTDIR}${ripaudit} \
	  $(DESTDIR)/${ripmirlog} ${DESTDIR}${ripsvrlog} \
	  $(DESTDIR)/${sqlog} ${DESTDIR}${ripupdlog} \
	  $(DESTDIR)/${updlog} ${DESTDIR}${acklog} \
	  $(DESTDIR)/${notiflog} ${DESTDIR}${forwlog}
	${INSTALL} -m 0755 ./utils/remadmin.pl $(DESTDIR)/${bindir}
	${INSTALL} -m 0755 ./utils/make_mysql_user $(DESTDIR)/${bindir}
	${INSTALL} -m 0755 ./utils/ripe2rpsl $(DESTDIR)/${bindir}
	${INSTALL} -m 0755 ./utils/make_db $(DESTDIR)/${bindir}
	${INSTALL} -m 0755 ./utils/load_all.sh $(DESTDIR)/${bindir}
	${INSTALL} -m 0755 ./utils/whoisd.server $(DESTDIR)/${bindir}
	${INSTALL} -m 0755 ./utils/whoisd_start $(DESTDIR)/${bindir}
	${INSTALL} -m 0644 ../conf/rip.config.sample $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/sources.config.sample $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/ack_template $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/ack_text $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/forw_create_template $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/frwd_template $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/frwd_text $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/notif_template $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../conf/notif_text $(DESTDIR)/${confdir}
	${INSTALL} -m 0644 ../doc/AUTO-DBM-HELP $(DESTDIR)/${docdir}
	${INSTALL} -m 0644 ../doc/WHOIS-HELP $(DESTDIR)/${docdir}
	${INSTALL} -m 0644 ../conf/sample.db.gz  $(DESTDIR)/${tmpdir}/load/SAMPLE
	${INSTALL} -m 0644 ../conf/SAMPLE.CURRENTSERIAL  $(DESTDIR)/${tmpdir}/load/SAMPLE
	${INSTALL} -m 0644 ./SQL/aux.index.2 $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0644 ./SQL/create.RIPADMIN.sql $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0644 ./SQL/create.tables.sql $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0644 ./SQL/leaf.index.2 $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0644 ./SQL/main.index.1 $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0755 ./SQL/create.first-stage $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0755 ./SQL/create.second-stage $(DESTDIR)/${bindir}/SQL
	${INSTALL} -m 0755 ./SQL/create.third-stage $(DESTDIR)/${bindir}/SQL
