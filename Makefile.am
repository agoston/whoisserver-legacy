SUBDIRS = src
AUTOMAKE_OPTIONS = subdir-objects foreign
EXTRA_DIST = rpsl.variant \
	     README \
	     README.MODULES \
	     README.freebsd \
	     README.linux \
	     README.solaris \
	     RELEASE-NOTES \
	     UPDATE-DB-3.2.0 \
	     WISHLIST \
	     conf/SAMPLE.CURRENTSERIAL \
	     conf/ack_template ack_text \
	     conf/delcheck-erx-ipv4-16 \
	     conf/delcheck-ipv4-16 \
	     conf/delcheck-ipv4-24 \
	     conf/delcheck-ipv6-32 \
	     conf/delcheck-ipv6-36 \
	     conf/delcheck-ipv6-48 \
	     conf/delcheck-template.xml \
	     conf/forw_create_template \
	     conf/frwd_template \
	     conf/frwd_text conf/notif_template \
	     conf/notif_text \
	     conf/rip.config.sample \
	     conf/rip.config.sample.q.in\
	     conf/sample.db.gz \
	     conf/sources.config.sample \
	     conf/sources.config.sample.q \
	     src/SQL/aux.index.2 \
	     src/SQL/create.RIPADMIN.sql \
	     src/SQL/create.RIPADMIN.sql.patch \
	     src/SQL/create.first-stage \
	     src/SQL/create.irt-tables.sql.patch \
	     src/SQL/create.last-history-indexes.sql.patch \
	     src/SQL/create.second-stage create.tables.sql \
	     src/SQL/create.tables.sql.patch create.third-stage \
	     src/SQL/fingerpr_x509.sql.patch leaf.index.2 \
	     src/SQL/main.index.1

install-exec-hook:
	${mkinstalldirs} ${DESTDIR}${topdir} ${DESTDIR}${bindir} \
	  ${DESTDIR}${confdir} \
	  ${DESTDIR}${docdir} \
	  ${DESTDIR}${bindir}/SQL \
	  ${DESTDIR}${vardir} ${DESTDIR}${tmpdir} \
	  ${DESTDIR}${tmpdir}/load ${DESTDIR}${tmpdir}/load/SAMPLE \
	  ${DESTDIR}${lockdir} ${DESTDIR}${logdir} \
	  ${DESTDIR}${qrylog} ${DESTDIR}${dbuperr} \
	  ${DESTDIR}${allriperr} ${DESTDIR}${ripaudit} \
	  ${DESTDIR}${ripmirlog} ${DESTDIR}${ripsvrlog} \
	  ${DESTDIR}${sqlog} ${DESTDIR}${ripupdlog} \
	  ${DESTDIR}${updlog} ${DESTDIR}${acklog} \
	  ${DESTDIR}${notiflog} ${DESTDIR}${forwlog}
	${INSTALL} -m 0755 ./src/utils/remadmin.pl ${DESTDIR}${bindir}
	${INSTALL} -m 0755 ./src/utils/make_mysql_user ${DESTDIR}${bindir}
	${INSTALL} -m 0755 ./src/utils/ripe2rpsl ${DESTDIR}${bindir}
	${INSTALL} -m 0755 ./src/utils/make_db ${DESTDIR}${bindir}
	${INSTALL} -m 0755 ./src/utils/load_all.sh ${DESTDIR}${bindir}
	${INSTALL} -m 0755 ./src/utils/whoisd.server ${DESTDIR}${bindir}
	${INSTALL} -m 0755 ./src/utils/whoisd_start ${DESTDIR}${bindir}
	${INSTALL} -m 0644 ./conf/rip.config.sample ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/sources.config.sample ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/rip.config.sample.q ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/sources.config.sample.q ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/ack_template ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/ack_text ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/forw_create_template ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/frwd_template ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/frwd_text ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/notif_template ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./conf/notif_text ${DESTDIR}${confdir}
	${INSTALL} -m 0644 ./doc/AUTO-DBM-HELP ${DESTDIR}${docdir}
	${INSTALL} -m 0644 ./doc/WHOIS-HELP ${DESTDIR}${docdir}
	${INSTALL} -m 0644 ./conf/sample.db.gz  ${DESTDIR}${tmpdir}/load/SAMPLE
	${INSTALL} -m 0644 ./conf/SAMPLE.CURRENTSERIAL  ${DESTDIR}${tmpdir}/load/SAMPLE
	${INSTALL} -m 0644 ./src/SQL/aux.index.2 ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0644 ./src/SQL/create.RIPADMIN.sql ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0644 ./src/SQL/create.tables.sql ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0644 ./src/SQL/leaf.index.2 ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0644 ./src/SQL/main.index.1 ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0755 ./src/SQL/create.first-stage ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0755 ./src/SQL/create.second-stage ${DESTDIR}/${bindir}/SQL
	${INSTALL} -m 0755 ./src/SQL/create.third-stage ${DESTDIR}/${bindir}/SQL
