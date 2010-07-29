/***************************************
 $Revision: 

 CA module: Macro definitions of the get and set functions.

 Status: NOT REVIEWED, NOT TESTED

 Author(s):       Ambrose Magee

 ******************//******************
 Modification History:

 ******************/

/************************************
 Copyright (c) 2000                              RIPE NCC

 All Rights Reserved
 
 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose and without fee is hereby granted,
 provided that the above copyright notice appear in all copies and that
 both that copyright notice and this permission notice appear in
 supporting documentation, and that the name of the author not be
 used in advertising or publicity pertaining to distribution of the
 software without specific, written prior permission.
 
 THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
 AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************/

#ifndef CA_MACROS
/* Include definitions of Macros. */
#define CA_MACROS

#include "ca_adminAttribs.h"

/* Macro defintions of the get and set functions. */

#define ca_get_bindport	ca_get_int(CA_BINDPORT)
#define ca_set_bindport	ca_set_int(CA_BINDPORT)

#define ca_get_help	ca_get_dirlist(CA_HELP)
#define ca_set_help	ca_set_dirlist(CA_HELP)

#define ca_get_replybanner	ca_get_string(CA_REPLYBANNER)
#define ca_set_replybanner	ca_set_string(CA_REPLYBANNER)

#define ca_get_source	ca_get_source(CA_SOURCE)
#define ca_set_source	ca_set_source(CA_SOURCE)

#define ca_get_testmode	ca_get_boolean(CA_TESTMODE)
#define ca_set_testmode	ca_set_boolean(CA_TESTMODE)

#define ca_get_overridecryptedpw	ca_get_string(CA_OVERRIDECRYPTEDPW)
#define ca_set_overridecryptedpw	ca_set_string(CA_OVERRIDECRYPTEDPW)

#define ca_get_overridepgp	ca_get_string(CA_OVERRIDEPGP)
#define ca_set_overridepgp	ca_set_string(CA_OVERRIDEPGP)

#define ca_get_whoishelp	ca_get_dirlist(CA_WHOISHELP)
#define ca_set_whoishelp	ca_set_dirlist(CA_WHOISHELP)

#define ca_get_attdesc	ca_get_dirlist(CA_ATTDESC)
#define ca_set_attdesc	ca_set_dirlist(CA_ATTDESC)

#define ca_get_deflook	ca_get_source(CA_DEFLOOK)
#define ca_set_deflook	ca_set_source(CA_DEFLOOK)

#define ca_get_dbfile	ca_get_dirlist(CA_DBFILE)
#define ca_set_dbfile	ca_set_dirlist(CA_DBFILE)

#define ca_get_canupd	ca_get_source(CA_CANUPD)
#define ca_set_canupd	ca_set_source(CA_CANUPD)

#define ca_get_uid	ca_get_int(CA_UID)
#define ca_set_uid	ca_set_int(CA_UID)

#define ca_get_qrylog	ca_get_dirlist(CA_QRYLOG)
#define ca_set_qrylog	ca_set_dirlist(CA_QRYLOG)

#define ca_get_ripmirlog	ca_get_dirlist(CA_RIPMIRLOG)
#define ca_set_ripmirlog	ca_set_dirlist(CA_RIPMIRLOG)

#define ca_get_ripsvrlog	ca_get_dirlist(CA_RIPSVRLOG)
#define ca_set_ripsvrlog	ca_set_dirlist(CA_RIPSVRLOG)

#define ca_get_sqlog	ca_get_dirlist(CA_SQLOG)
#define ca_set_sqlog	ca_set_dirlist(CA_SQLOG)

#define ca_get_qry2log	ca_get_dirlist(CA_QRY2LOG)
#define ca_set_qry2log	ca_set_dirlist(CA_QRY2LOG)

#define ca_get_errlog	ca_get_dirlist(CA_ERRLOG)
#define ca_set_errlog	ca_set_dirlist(CA_ERRLOG)

#define ca_get_ripaudit	ca_get_dirlist(CA_RIPAUDIT)
#define ca_set_ripaudit	ca_set_dirlist(CA_RIPAUDIT)

#define ca_get_dberrlog       ca_get_dirlist(CA_DBUPERR)
#define ca_set_dberrlog       ca_set_dirlist(CA_DBUPERR)

#define ca_get_allriperr	ca_get_dirlist(CA_ALLRIPERR)
#define ca_set_allriperr	ca_set_dirlist(CA_ALLRIPERR)

#define ca_get_ripupdlog	ca_get_dirlist(CA_RIPUPDLOG)
#define ca_set_ripupdlog	ca_set_dirlist(CA_RIPUPDLOG)

#define ca_get_updlog	ca_get_dirlist(CA_UPDLOG)
#define ca_set_updlog	ca_set_dirlist(CA_UPDLOG)

#define ca_get_acklog	ca_get_dirlist(CA_ACKLOG)
#define ca_set_acklog	ca_set_dirlist(CA_ACKLOG)

#define ca_get_ack_template	ca_get_dirlist(CA_ACKTEMPLATE)
#define ca_get_notif_template	ca_get_dirlist(CA_NOTIFTEMPLATE)
#define ca_get_frwd_template	ca_get_dirlist(CA_FRWDTEMPLATE)
#define ca_get_forw_create_template	ca_get_dirlist(CA_FORWCREATETEMPLATE)

#define ca_get_lockdir	ca_get_dirlist(CA_LOCKDIR)
#define ca_set_lockdir	ca_set_dirlist(CA_LOCKDIR)

#define ca_get_pidfile	ca_get_string(CA_PIDFILE)
#define ca_set_pidfile	ca_set_string(CA_PIDFILE)

#define ca_get_updatelock	ca_get_string(CA_UPDATELOCK)
#define ca_set_updatelock	ca_set_string(CA_UPDATELOCK)

#define ca_get_cleanlock	ca_get_string(CA_CLEANLOCK)
#define ca_set_cleanlock	ca_set_string(CA_CLEANLOCK)

#define ca_get_serialdir	ca_get_dirlist(CA_SERIALDIR)
#define ca_set_serialdir	ca_set_dirlist(CA_SERIALDIR)

#define ca_get_oldserialdir	ca_get_dirlist(CA_OLDSERIALDIR)
#define ca_set_oldserialdir	ca_set_dirlist(CA_OLDSERIALDIR)

#define ca_get_serialincomingdir	ca_get_dirlist(CA_SERIALINCOMINGDIR)
#define ca_set_serialincomingdir	ca_set_dirlist(CA_SERIALINCOMINGDIR)

#define ca_get_keepopen	ca_get_int(CA_KEEPOPEN)
#define ca_set_keepopen	ca_set_int(CA_KEEPOPEN)

#define ca_get_allocmnt	ca_get_string(CA_ALLOCMNT)
#define ca_set_allocmnt	ca_set_string(CA_ALLOCMNT)

#define ca_get_disallowmnt	ca_get_string(CA_DISALLOWMNT)
#define ca_set_disallowmnt	ca_set_string(CA_DISALLOWMNT)

#define ca_get_org_power_mnt ca_get_string(CA_ORG_POWER_MNT)
#define ca_set_org_power_mnt ca_set_string(CA_ORG_POWER_MNT)

#define ca_get_getupdatesfrom	ca_get_string(CA_GETUPDATESFROM)
#define ca_set_getupdatesfrom	ca_set_string(CA_GETUPDATESFROM)

#define ca_get_denywhoisaccess	ca_get_string(CA_DENYWHOISACCESS)
#define ca_set_denywhoisaccess	ca_set_string(CA_DENYWHOISACCESS)

#define ca_get_tmpdir	ca_get_dirlist(CA_TMPDIR)
#define ca_set_tmpdir	ca_set_dirlist(CA_TMPDIR)

#define ca_get_processuser	ca_get_string(CA_PROCESSUSER)
#define ca_set_processuser	ca_set_string(CA_PROCESSUSER)

#define ca_get_gpgcmd	ca_get_dirlist(CA_GPGCMD)
#define ca_set_gpgcmd	ca_set_dirlist(CA_GPGCMD)

#define ca_get_opensslcmd	ca_get_dirlist(CA_OPENSSLCMD)
#define ca_set_opensslcmd	ca_set_dirlist(CA_OPENSSLCMD)

#define ca_get_crllist	ca_get_dirlist(CA_CRLLIST)
#define ca_set_crllist	ca_set_dirlist(CA_CRLLIST)

#define ca_get_pgppath	ca_get_dirlist(CA_PGPPATH)
#define ca_set_pgppath	ca_set_dirlist(CA_PGPPATH)

#define ca_get_cacerts	ca_get_dirlist(CA_CACERTS)
#define ca_set_cacerts	ca_set_dirlist(CA_CACERTS)

#define ca_get_mailcmd	ca_get_dirlist(CA_MAILCMD)
#define ca_set_mailcmd	ca_set_dirlist(CA_MAILCMD)

#define ca_get_overflowsize	ca_get_int(CA_OVERFLOWSIZE)
#define ca_set_overflowsize	ca_set_int(CA_OVERFLOWSIZE)

#define ca_get_dbcachesize	ca_get_int(CA_DBCACHESIZE)
#define ca_set_dbcachesize	ca_set_int(CA_DBCACHESIZE)

#define ca_get_nrofnames	ca_get_int(CA_NROFNAMES)
#define ca_set_nrofnames	ca_set_int(CA_NROFNAMES)

#define ca_get_defmail	ca_get_string(CA_DEFMAIL)
#define ca_set_defmail	ca_set_string(CA_DEFMAIL)

#define ca_get_humailbox	ca_get_string(CA_HUMAILBOX)
#define ca_set_humailbox	ca_set_string(CA_HUMAILBOX)

#define ca_get_autobox	ca_get_string(CA_AUTOBOX)
#define ca_set_autobox	ca_set_string(CA_AUTOBOX)

#define ca_get_replybanner	ca_get_string(CA_REPLYBANNER)
#define ca_set_replybanner	ca_set_string(CA_REPLYBANNER)

#define ca_get_nomatch	ca_get_string(CA_NOMATCH)
#define ca_set_nomatch	ca_set_string(CA_NOMATCH)

#define ca_get_denyaccesstxt	ca_get_string(CA_DENYACCESSTXT)
#define ca_set_denyaccesstxt	ca_set_string(CA_DENYACCESSTXT)

#define ca_get_referraltxt	ca_get_string(CA_REFERRALTXT)
#define ca_set_referraltxt	ca_set_string(CA_REFERRALTXT)

#define ca_get_referralerrortxt	ca_get_string(CA_REFERRALERRORTXT)
#define ca_set_referralerrortxt	ca_set_string(CA_REFERRALERRORTXT)

#define ca_get_referrallooperrortxt	ca_get_string(CA_REFERRALLOOPERRORTXT)
#define ca_set_referrallooperrortxt	ca_set_string(CA_REFERRALLOOPERRORTXT)

#define ca_get_referraltimeouttxt	ca_get_string(CA_REFERRALTIMEOUTTXT)
#define ca_set_referraltimeouttxt	ca_set_string(CA_REFERRALTIMEOUTTXT)

#define ca_get_referraltrunctxt	ca_get_string(CA_REFERRALTRUNCTXT)
#define ca_set_referraltrunctxt	ca_set_string(CA_REFERRALTRUNCTXT)

#define ca_get_referralendtxt	ca_get_string(CA_REFERRALENDTXT)
#define ca_set_referralendtxt	ca_set_string(CA_REFERRALENDTXT)

#define ca_get_referraltimeout	ca_get_int(CA_REFERRALTIMEOUT)
#define ca_set_referraltimeout	ca_set_int(CA_REFERRALTIMEOUT)

#define ca_get_referralmaxlines	ca_get_int(CA_REFERRALMAXLINES)
#define ca_set_referralmaxlines	ca_set_int(CA_REFERRALMAXLINES)

#define ca_get_rights	ca_get_string(CA_RIGHTS)
#define ca_set_rights	ca_set_string(CA_RIGHTS)

#define ca_get_nicsuffix	ca_get_string(CA_NICSUFFIX)
#define ca_set_nicsuffix	ca_set_string(CA_NICSUFFIX)

#define ca_get_country	ca_get_string(CA_COUNTRY)
#define ca_set_country	ca_set_string(CA_COUNTRY)

#define ca_get_authmethod	ca_get_string(CA_AUTHMETHOD)
#define ca_set_authmethod	ca_set_string(CA_AUTHMETHOD)

#define ca_get_dictionary ca_getDictionary(dictionary, VARS)

#define ca_get_srcname(X)	ca_srchandle2Strelement((X), CA_SRCNAME)

#define ca_get_srccanupd(X)	ca_srchandle2Strelement((X), CA_DBCANUPD)

#define ca_get_srcdeflook(X) ca_srchandle2Strelement((X), CA_DBDEFLOOK)

#define ca_get_srcdbmachine(X) ca_srchandle2Strelement((X), CA_DBMACHINE)

#define ca_get_srcdbuser(X) ca_srchandle2Strelement((X), CA_DBUSER)

#define ca_get_srcdbpassword(X) ca_srchandle2Strelement((X), CA_DBPASSWORD)

#define ca_get_srcdbname(X) ca_srchandle2Strelement((X), CA_DBNAME)

#define ca_get_srcdbport(X) ca_srchandle2Intelement((X), CA_DBPORT)

/* the following added 25/10/2002 EG */
#define ca_get_srctrx_support(X) ca_srchandle2Strelement((X), CA_DBTRX_SUPPORT)

#define ca_get_srcnrtmhost(X) ca_srchandle2Strelement((X), CA_NRTMHOST)

#define ca_get_srcnrtmlog(X) ca_srchandle2Strelement((X), CA_NRTMLOG)

#define ca_get_srcmode(X) ca_srchandle2Intelement((X), CA_SRCMODE)

#define ca_get_srcnrtmport(X) ca_srchandle2Intelement((X), CA_NRTMPORT)

#define ca_get_srcnrtmdelay(X) ca_srchandle2Intelement((X), CA_NRTMDELAY)

#define ca_get_srcnrtmprotocolvers(X) ca_srchandle2Intelement((X), CA_NRTMPROTOCOLVERS)

#define ca_get_srcupdateport(X) ca_srchandle2Intelement((X), CA_SRCUPDPORT)

#define ca_get_notiflog ca_get_dirlist(CA_NOTIFLOG)
#define ca_set_notiflog ca_set_dirlist(CA_NOTIFLOG)

#define ca_get_forwlog  ca_get_dirlist(CA_FORWLOG)
#define ca_set_forwlog  ca_set_dirlist(CA_FORWLOG)

#define ca_get_ripadminport ca_get_adminIntElement(CA_RIPADMIN, CA_ADMINPORT)
#define ca_get_ripadminuser ca_get_adminStrElement(CA_RIPADMIN, CA_ADMINUSER)

#define ca_get_ripadmintable ca_get_adminStrElement(CA_RIPADMIN, CA_ADMINTABLE)

#define ca_get_ripadminhost ca_get_adminStrElement(CA_RIPADMIN, CA_ADMINHOST)

#define ca_get_ripadminpassword ca_get_adminStrElement(CA_RIPADMIN, CA_ADMINPASSWORD)

#define ca_get_updsrcname(X)	ca_UpdSrcHandle2StrElement((X), CA_SRCNAME)

#define ca_get_updsrcdbmachine(X)	ca_UpdSrcHandle2StrElement((X), CA_DBMACHINE)

#define ca_get_updsrcdbuser(X) ca_UpdSrcHandle2StrElement((X), CA_DBUSER)

#define ca_get_updsrcdbpassword(X) ca_UpdSrcHandle2StrElement((X), CA_DBPASSWORD)

#define ca_get_updsrcdbname(X)	ca_UpdSrcHandle2StrElement((X), CA_DBNAME)

#define ca_get_updsrcwhoishost(X) ca_UpdSrcHandle2StrElement((X), CA_UPDSRCWHOISHOST)

#define ca_get_updsrcdbport(X) ca_UpdSrcHandle2IntElement((X), CA_DBPORT)
#define ca_get_updqryport(X) ca_UpdSrcHandle2IntElement((X), CA_UPDSRCQRYPORT)

#define ca_get_updupdport(X) ca_UpdSrcHandle2IntElement((X), CA_UPDSRCUPDPORT)

#define ca_get_svwhois_port	ca_get_int(CA_SVWHOIS_PORT)
#define ca_set_svwhois_port	ca_set_int(CA_SVWHOIS_PORT)

#define ca_get_svconfig_port	ca_get_int(CA_SVCONFIG_PORT)
#define ca_set_svconfig_port	ca_set_int(CA_SVCONFIG_PORT)

#define ca_get_svmirror_port	ca_get_int(CA_SVMIRROR_PORT)
#define ca_set_svmirror_port	ca_set_int(CA_SVMIRROR_PORT)

#define ca_get_svlookup_port	ca_get_int(CA_SVLOOKUP_PORT)
#define ca_set_svlookup_port	ca_set_int(CA_SVLOOKUP_PORT)

#define ca_get_sourcefile ca_get_dirlist(CA_SOURCEFILE)
#define ca_set_sourcefile ca_set_dirlist(CA_SOURCEFILE)

#define ca_get_ac_decay_interval ca_get_int(CA_AC_DECAY_INTERVAL)
#define ca_set_ac_decay_interval ca_set_int(CA_AC_DECAY_INTERVAL)

#define ca_get_ac_decay_halflife ca_get_int(CA_AC_DECAY_HALFLIFE)
#define ca_set_ac_decay_halflife ca_set_int(CA_AC_DECAY_HALFLIFE)

#define ca_get_ac_save_interval ca_get_int(CA_AC_SAVE_INTERVAL)
#define ca_set_ac_save_interval ca_set_int(CA_AC_SAVE_INTERVAL)

#define ca_get_ac_auto_save ca_get_boolean(CA_AC_AUTO_SAVE)
#define ca_set_ac_auto_save ca_set_boolean(CA_AC_AUTO_SAVE)

#define ca_get_ac_load ca_get_boolean(CA_AC_LOAD)
#define ca_set_ac_load ca_set_boolean(CA_AC_LOAD)

#define ca_get_pw_resp_header	ca_get_string(CA_PW_RESP_HEADER)
#define ca_set_pw_resp_header	ca_set_string(CA_PW_RESP_HEADER)

#define ca_get_pw_err_nokey	ca_get_string(CA_PW_ERR_NOKEY)
#define ca_set_pw_err_nokey	ca_set_string(CA_PW_ERR_NOKEY)

#define ca_get_pw_err_linetoolong	ca_get_string(CA_PW_ERR_LINETOOLONG)
#define ca_set_pw_err_linetoolong	ca_set_string(CA_PW_ERR_LINETOOLONG)

#define ca_get_pw_help_file	ca_get_dirlist(CA_PW_HELP_FILE)
#define ca_set_pw_help_file	ca_set_dirlist(CA_PW_HELP_FILE)

#define ca_get_pw_k_filter	ca_get_string(CA_PW_K_FILTER)
#define ca_set_pw_k_filter	ca_set_string(CA_PW_K_FILTER)

#define ca_get_pw_fmt_limit_reached	ca_get_string(CA_PW_FMT_LIMIT_REACHED)
#define ca_set_pw_fmt_limit_reached	ca_set_string(CA_PW_FMT_LIMIT_REACHED)

#define ca_get_pw_fmt_acl_addrpass	ca_get_string(CA_PW_FMT_ACL_ADDRPASS)
#define ca_set_pw_fmt_acl_addrpass	ca_set_string(CA_PW_FMT_ACL_ADDRPASS)

#define ca_get_pw_fmt_acl_permdeny	ca_get_string(CA_PW_FMT_ACL_PERMDENY)
#define ca_set_pw_fmt_acl_permdeny	ca_set_string(CA_PW_FMT_ACL_PERMDENY)

#define ca_get_pw_fmt_acl_conndeny	ca_get_string(CA_PW_FMT_ACL_CONNDENY)
#define ca_set_pw_fmt_acl_conndeny	ca_set_string(CA_PW_FMT_ACL_CONNDENY)

#define ca_get_pw_notfound	ca_get_string(CA_PW_NOTFOUND)
#define ca_set_pw_notfound	ca_set_string(CA_PW_NOTFOUND)

#define ca_get_pw_connclosed	ca_get_string(CA_PW_CONNCLOSED)
#define ca_set_pw_connclosed	ca_set_string(CA_PW_CONNCLOSED)

#define ca_get_pw_banner	ca_get_string(CA_PW_BANNER)
#define ca_set_pw_banner	ca_set_string(CA_PW_BANNER)

#define ca_get_qc_fmt_badattr	ca_get_string(CA_QC_FMT_BADATTR)
#define ca_set_qc_fmt_badattr	ca_set_string(CA_QC_FMT_BADATTR)

#define ca_get_qc_fmt_attrnotinv	ca_get_string(CA_QC_FMT_ATTRNOTINV)
#define ca_set_qc_fmt_attrnotinv	ca_set_string(CA_QC_FMT_ATTRNOTINV)

#define ca_get_qc_fmt_badsource	ca_get_string(CA_QC_FMT_BADSOURCE)
#define ca_set_qc_fmt_badsource	ca_set_string(CA_QC_FMT_BADSOURCE)

#define ca_get_qc_fmt_badobjtype	ca_get_string(CA_QC_FMT_BADOBJTYPE)
#define ca_set_qc_fmt_badobjtype	ca_set_string(CA_QC_FMT_BADOBJTYPE)

#define ca_get_qc_fmt_dupipflag	ca_get_string(CA_QC_FMT_DUPIPFLAG)
#define ca_set_qc_fmt_dupipflag	ca_set_string(CA_QC_FMT_DUPIPFLAG)

#define ca_get_qc_dupproxyipflag	ca_get_string(CA_QC_DUPPROXYIPFLAG)
#define ca_set_qc_dupproxyipflag	ca_set_string(CA_QC_DUPPROXYIPFLAG)

#define ca_get_qc_badinput	ca_get_string(CA_QC_BADINPUT)
#define ca_set_qc_badinput	ca_set_string(CA_QC_BADINPUT)

#define ca_get_qi_badrange	ca_get_string(CA_QI_BADRANGE)
#define ca_set_qi_badrange	ca_set_string(CA_QI_BADRANGE)

#define ca_get_qc_uselessipflag	ca_get_string(CA_QC_USELESSIPFLAG)
#define ca_set_qc_uselessipflag	ca_set_string(CA_QC_USELESSIPFLAG)

#define ca_get_qc_uselessnorefflag	ca_get_string(CA_QC_USELESSNOREFFLAG)
#define ca_set_qc_uselessnorefflag	ca_set_string(CA_QC_USELESSNOREFFLAG)

#define ca_get_qc_trailingdotindomain	ca_get_string(CA_QC_TRAILINGDOTINDOMAIN)
#define ca_set_qc_trailingdotindomain	ca_set_string(CA_QC_TRAILINGDOTINDOMAIN)

#define ca_get_qc_fmt_fixedlookup	ca_get_string(CA_QC_FMT_FIXEDLOOKUP)
#define ca_set_qc_fmt_fixedlookup	ca_set_string(CA_QC_FMT_FIXEDLOOKUP)

#define ca_get_qc_fmt_uncompflag	ca_get_string(CA_QC_FMT_UNCOMPFLAG)
#define ca_set_qc_fmt_uncompflag	ca_set_string(CA_QC_FMT_UNCOMPFLAG)

#define ca_get_qi_ref_tmout	ca_get_string(CA_QI_REF_TMOUT)
#define ca_set_qi_ref_tmout	ca_set_string(CA_QI_REF_TMOUT)

#define ca_get_qi_ref_overmaxlin	ca_get_string(CA_QI_REF_OVERMAXLIN)
#define ca_set_qi_ref_overmaxlin	ca_set_string(CA_QI_REF_OVERMAXLIN)

#define ca_get_qi_ref_badhost	ca_get_string(CA_QI_REF_BADHOST)
#define ca_set_qi_ref_badhost	ca_set_string(CA_QI_REF_BADHOST)

#define ca_get_qi_ref_hostnottresp	ca_get_string(CA_QI_REF_HOSTNOTTRESP)
#define ca_set_qi_ref_hostnottresp	ca_set_string(CA_QI_REF_HOSTNOTTRESP)

#define ca_get_qi_fmt_refheader	ca_get_string(CA_QI_FMT_REFHEADER)
#define ca_set_qi_fmt_refheader	ca_set_string(CA_QI_FMT_REFHEADER)

#define ca_get_qi_fmt_group_banner ca_get_string(CA_QI_FMT_GROUP_BANNER)
#define ca_set_qi_fmt_group_banner ca_set_string(CA_QI_FMT_GROUP_BANNER)

#define ca_get_qi_filter_banner ca_get_string(CA_QI_FILTER_BANNER)
#define ca_set_qi_filter_banner ca_set_string(CA_QI_FILTER_BANNER)

#define ca_get_er_ud_def	ca_get_dirlist(CA_ER_UD_DEF)
#define ca_set_er_ud_def	ca_set_dirlist(CA_ER_UD_DEF)

#define ca_get_qi_reftrailer ca_get_string(CA_QI_REFTRAILER)
#define ca_set_qi_reftrailer ca_set_string(CA_QI_REFTRAILER)

#define ca_get_ns_rir      ca_get_string(CA_NS_RIR)
#define ca_set_ns_rir      ca_set_string(CA_NS_RIR)

#define ca_get_ns_suffix      ca_get_string(CA_NS_SUFFIX)
#define ca_set_ns_suffix      ca_set_string(CA_NS_SUFFIX)

#define ca_get_ns_nservers      ca_get_string(CA_NS_NSERVERS)
#define ca_set_ns_nservers      ca_set_string(CA_NS_NSERVERS)

#define ca_get_ns_delcheckconf      ca_get_string(CA_NS_DELCHECKCONF)
#define ca_set_ns_delcheckconf      ca_set_string(CA_NS_DELCHECKCONF)

#define ca_get_ns_delegationsfile	ca_get_dirlist(CA_NS_DELEGATIONSFILE)
#define ca_set_ns_delegationsfile	ca_set_dirlist(CA_NS_DELEGATIONSFILE)

#define ca_get_ns_severity	ca_get_int(CA_NS_SEVERITY)
#define ca_set_ns_severity	ca_set_int(CA_NS_SEVERITY)

#define ca_get_command_on_die	ca_get_string(CA_COMMAND_ON_DIE)
#define ca_set_command_on_die	ca_set_string(CA_COMMAND_ON_DIE)

#define ca_get_dummy_add_attr ca_get_string(CA_DUMMY_ADD_ATTR)
#define ca_set_dummy_add_attr ca_set_string(CA_DUMMY_ADD_ATTR)

#define ca_get_operemail ca_get_string(CA_OPEREMAIL)
#define ca_set_operemail ca_set_string(CA_OPEREMAIL)

#define ca_get_nrtm_history_access_limit ca_get_int(CA_NRTM_HISTORY_ACCESS_LIMIT)
#define ca_set_nrtm_history_access_limit ca_set_int(CA_NRTM_HISTORY_ACCESS_LIMIT)

#endif	/* CA_MACROS */

