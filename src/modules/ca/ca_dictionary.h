/***************************************
  $Revision:

  CA module: a wrapper for the functions which parse the
             dictionary, configuration and source files.

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


/* m4 macro */


#ifndef CA_DICTIONARY
#define CA_DICTIONARY

#define CAP_W 0x000001
#define CAP_U 0x000002



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
  
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

typedef enum
{
  CA_TESTMODE,
  CA_OVERRIDECRYPTEDPW,
  CA_OVERRIDEPGP,
  CA_QRYLOG,
  CA_DBUPERR,
  CA_ALLRIPERR,
  CA_RIPAUDIT,
  CA_RIPMIRLOG,
  CA_RIPSVRLOG,
  CA_SQLOG,
  CA_RIPUPDLOG,
  CA_UPDLOG,
  CA_ACKLOG,
  CA_ACKTEMPLATE,
  CA_NOTIFTEMPLATE,
  CA_FRWDTEMPLATE,
  CA_FORWCREATETEMPLATE,
  CA_LOCKDIR,
  CA_UPDATELOCK,
  CA_KEEPOPEN,
  CA_ALLOCMNT,
  CA_DISALLOWMNT,
  CA_ORG_POWER_MNT,
  CA_TMPDIR,
  CA_SAWFILE,
  CA_RIR,
  CA_GPGCMD,
  CA_OPENSSLCMD,
  CA_CRLLIST,
  CA_PGPPATH,
  CA_CACERTS,
  CA_MAILCMD,
  CA_HUMAILBOX,
  CA_AUTOBOX,
  CA_DEFMAIL,
  CA_AUTHMETHOD,
  CA_REFERRALTIMEOUT,
  CA_REFERRALMAXLINES,
  CA_NICSUFFIX,
  CA_COUNTRY,
  CA_RIPADMIN,
  CA_NOTIFLOG,
  CA_FORWLOG,
  CA_UPDSOURCE,
  CA_SVWHOIS_PORT,
  CA_SVCONFIG_PORT,
  CA_SVMIRROR_PORT,
  CA_SOURCEFILE,
  CA_AC_DECAY_INTERVAL,
  CA_AC_DECAY_HALFLIFE,
  CA_AC_LOAD,
  CA_AC_AUTO_SAVE,
  CA_AC_SAVE_INTERVAL,
  CA_PW_RESP_HEADER,
  CA_PW_ERR_NOKEY,
  CA_PW_HELP_FILE,
  CA_PW_K_FILTER,
  CA_PW_FMT_LIMIT_REACHED,
  CA_PW_FMT_ACL_ADDRPASS,
  CA_PW_FMT_ACL_PERMDENY,
  CA_PW_FMT_ACL_CONNDENY,
  CA_PW_NOTFOUND,
  CA_PW_CONNCLOSED,
  CA_PW_BANNER,
  CA_PW_ERR_LINETOOLONG,
  CA_QC_FMT_ATTRNOTINV,
  CA_QC_FMT_BADSOURCE,
  CA_QC_FMT_BADATTR,
  CA_QC_FMT_BADOBJTYPE,
  CA_QC_FMT_DUPIPFLAG,
  CA_QC_DUPPROXYIPFLAG,
  CA_QC_USELESSIPFLAG,
  CA_QC_FMT_FIXEDLOOKUP,
  CA_QC_FMT_UNCOMPFLAG,
  CA_QC_USELESSNOREFFLAG,
  CA_QC_BADINPUT,
  CA_QI_REF_TMOUT,
  CA_QI_REF_OVERMAXLIN,
  CA_QI_REF_BADHOST,
  CA_QI_REF_HOSTNOTTRESP,
  CA_QI_FMT_REFHEADER,
	CA_QI_FMT_GROUP_BANNER,
	CA_QI_FILTER_BANNER,
  CA_QI_REFTRAILER,
  CA_NS_RIR,
  CA_NS_DELEGATIONSFILE,
  CA_NS_SEVERITY,
  CA_NS_SUFFIX,
  CA_NS_NSERVERS,
  CA_NS_DELCHECKCONF,


  CA_NUMBEROFSYMBOLS

} ca_dictsym;
 
#ifndef DICT_INIT
extern dict_t dictionary[];
#else 
dict_t dictionary[] = 
{
 {"TESTMODE", "CA_TESTMODE",  "CA_BOOLEAN", CA_TESTMODE, 1},
 {"OVERRIDECRYPTEDPW", "CA_OVERRIDECRYPTEDPW",  "CA_STRING", CA_OVERRIDECRYPTEDPW, 1},
 {"OVERRIDEPGP", "CA_OVERRIDEPGP",  "CA_STRING", CA_OVERRIDEPGP, 1},
 {"QRYLOG", "CA_QRYLOG",  "CA_DIRLIST", CA_QRYLOG, 1},
 {"DBUPERR", "CA_DBUPERR",  "CA_DIRLIST", CA_DBUPERR, 1},
 {"ALLRIPERR", "CA_ALLRIPERR",  "CA_DIRLIST", CA_ALLRIPERR, 1},
 {"RIPAUDIT", "CA_RIPAUDIT",  "CA_DIRLIST", CA_RIPAUDIT, 1},
 {"RIPMIRLOG", "CA_RIPMIRLOG",  "CA_DIRLIST", CA_RIPMIRLOG, 1},
 {"RIPSVRLOG", "CA_RIPSVRLOG",  "CA_DIRLIST", CA_RIPSVRLOG, 1},
 {"SQLOG", "CA_SQLOG",  "CA_DIRLIST", CA_SQLOG, 1},
 {"RIPUPDLOG", "CA_RIPUPDLOG",  "CA_DIRLIST", CA_RIPUPDLOG, 1},
 {"UPDLOG", "CA_UPDLOG",  "CA_DIRLIST", CA_UPDLOG, 1},
 {"ACKLOG", "CA_ACKLOG",  "CA_DIRLIST", CA_ACKLOG, 1},
 {"ACKTEMPLATE", "CA_ACKTEMPLATE",  "CA_DIRLIST", CA_ACKTEMPLATE, 1},
 {"NOTIFTEMPLATE", "CA_NOTIFTEMPLATE",  "CA_DIRLIST", CA_NOTIFTEMPLATE, 1},
 {"FRWDTEMPLATE", "CA_FRWDTEMPLATE",  "CA_DIRLIST", CA_FRWDTEMPLATE, 1},
 {"FORWCREATETEMPLATE", "CA_FORWCREATETEMPLATE",  "CA_DIRLIST", CA_FORWCREATETEMPLATE, 1},
 {"LOCKDIR", "CA_LOCKDIR",  "CA_DIRLIST", CA_LOCKDIR, 1},
 {"UPDATELOCK", "CA_UPDATELOCK",  "CA_STRING", CA_UPDATELOCK, 1},
 {"KEEPOPEN", "CA_KEEPOPEN",  "CA_INT", CA_KEEPOPEN, 1},
 {"ALLOCMNT", "CA_ALLOCMNT",  "CA_STRING", CA_ALLOCMNT, 1},
 {"DISALLOWMNT", "CA_DISALLOWMNT",  "CA_STRING", CA_DISALLOWMNT, 1},
 {"ORG_POWER_MNT", "CA_ORG_POWER_MNT",  "CA_STRING", CA_ORG_POWER_MNT, 1},
 {"TMPDIR", "CA_TMPDIR",  "CA_DIRLIST", CA_TMPDIR, 1},
 {"SAWFILE", "CA_SAWFILE",  "CA_DIRLIST", CA_SAWFILE, 1},
 {"RIR", "CA_RIR",  "CA_STRING", CA_RIR, 1},
 {"GPGCMD", "CA_GPGCMD",  "CA_DIRLIST", CA_GPGCMD, 1},
 {"OPENSSLCMD", "CA_OPENSSLCMD",  "CA_DIRLIST", CA_OPENSSLCMD, 1},
 {"CRLLIST", "CA_CRLLIST",  "CA_DIRLIST", CA_CRLLIST, 1},
 {"PGPPATH", "CA_PGPPATH",  "CA_DIRLIST", CA_PGPPATH, 1},
 {"CACERTS", "CA_CACERTS",  "CA_DIRLIST", CA_CACERTS, 1},
 {"MAILCMD", "CA_MAILCMD",  "CA_DIRLIST", CA_MAILCMD, 1},
 {"HUMAILBOX", "CA_HUMAILBOX",  "CA_STRING", CA_HUMAILBOX, 1},
 {"AUTOBOX", "CA_AUTOBOX",  "CA_STRING", CA_AUTOBOX, 1},
 {"DEFMAIL", "CA_DEFMAIL",  "CA_STRING", CA_DEFMAIL, 1},
 {"AUTHMETHOD", "CA_AUTHMETHOD",  "CA_STRING", CA_AUTHMETHOD, 1},
 {"REFERRALTIMEOUT", "CA_REFERRALTIMEOUT",  "CA_INT", CA_REFERRALTIMEOUT, 1},
 {"REFERRALMAXLINES", "CA_REFERRALMAXLINES",  "CA_INT", CA_REFERRALMAXLINES, 1},
 {"NICSUFFIX", "CA_NICSUFFIX",  "CA_STRING", CA_NICSUFFIX, 1},
 {"COUNTRY", "CA_COUNTRY",  "CA_STRING", CA_COUNTRY, 1},
 {"RIPADMIN", "CA_RIPADMIN",  "CA_ADMIN", CA_RIPADMIN, 1},
 {"NOTIFLOG", "CA_NOTIFLOG",  "CA_DIRLIST", CA_NOTIFLOG, 1},
 {"FORWLOG", "CA_FORWLOG",  "CA_DIRLIST", CA_FORWLOG, 1},
 {"UPDSOURCE", "CA_UPDSOURCE",  "CA_UPDSOURCE", CA_UPDSOURCE, 1},
 {"SVWHOIS_PORT", "CA_SVWHOIS_PORT",  "CA_INT", CA_SVWHOIS_PORT, 1},
 {"SVCONFIG_PORT", "CA_SVCONFIG_PORT",  "CA_INT", CA_SVCONFIG_PORT, 1},
 {"SVMIRROR_PORT", "CA_SVMIRROR_PORT",  "CA_INT", CA_SVMIRROR_PORT, 1},
 {"SOURCEFILE", "CA_SOURCEFILE",  "CA_DIRLIST", CA_SOURCEFILE, 1},
 {"AC_DECAY_INTERVAL", "CA_AC_DECAY_INTERVAL",  "CA_INT", CA_AC_DECAY_INTERVAL, 1},
 {"AC_DECAY_HALFLIFE", "CA_AC_DECAY_HALFLIFE",  "CA_INT", CA_AC_DECAY_HALFLIFE, 1},
 {"AC_LOAD", "CA_AC_LOAD",  "CA_BOOLEAN", CA_AC_LOAD, 1},
 {"AC_AUTO_SAVE", "CA_AC_AUTO_SAVE",  "CA_BOOLEAN", CA_AC_AUTO_SAVE, 1},
 {"AC_SAVE_INTERVAL", "CA_AC_SAVE_INTERVAL",  "CA_INT", CA_AC_SAVE_INTERVAL, 1},
 {"PW_RESP_HEADER", "CA_PW_RESP_HEADER",  "CA_STRING", CA_PW_RESP_HEADER, 1},
 {"PW_ERR_NOKEY", "CA_PW_ERR_NOKEY",  "CA_STRING", CA_PW_ERR_NOKEY, 1},
 {"PW_HELP_FILE", "CA_PW_HELP_FILE",  "CA_DIRLIST", CA_PW_HELP_FILE, 1},
 {"PW_K_FILTER", "CA_PW_K_FILTER",  "CA_STRING", CA_PW_K_FILTER, 1},
 {"PW_FMT_LIMIT_REACHED", "CA_PW_FMT_LIMIT_REACHED",  "CA_STRING", CA_PW_FMT_LIMIT_REACHED, 1},
 {"PW_FMT_ACL_ADDRPASS", "CA_PW_FMT_ACL_ADDRPASS",  "CA_STRING", CA_PW_FMT_ACL_ADDRPASS, 1},
 {"PW_FMT_ACL_PERMDENY", "CA_PW_FMT_ACL_PERMDENY",  "CA_STRING", CA_PW_FMT_ACL_PERMDENY, 1},
 {"PW_FMT_ACL_CONNDENY", "CA_PW_FMT_ACL_CONNDENY",  "CA_STRING", CA_PW_FMT_ACL_CONNDENY, 1},
 {"PW_NOTFOUND", "CA_PW_NOTFOUND",  "CA_STRING", CA_PW_NOTFOUND, 1},
 {"PW_CONNCLOSED", "CA_PW_CONNCLOSED",  "CA_STRING", CA_PW_CONNCLOSED, 1},
 {"PW_BANNER", "CA_PW_BANNER",  "CA_STRING", CA_PW_BANNER, 1},
 {"PW_ERR_LINETOOLONG", "CA_PW_ERR_LINETOOLONG",  "CA_STRING", CA_PW_ERR_LINETOOLONG, 1},
 {"QC_FMT_ATTRNOTINV", "CA_QC_FMT_ATTRNOTINV",  "CA_STRING", CA_QC_FMT_ATTRNOTINV, 1},
 {"QC_FMT_BADSOURCE", "CA_QC_FMT_BADSOURCE",  "CA_STRING", CA_QC_FMT_BADSOURCE, 1},
 {"QC_FMT_BADATTR", "CA_QC_FMT_BADATTR",  "CA_STRING", CA_QC_FMT_BADATTR, 1},
 {"QC_FMT_BADOBJTYPE", "CA_QC_FMT_BADOBJTYPE",  "CA_STRING", CA_QC_FMT_BADOBJTYPE, 1},
 {"QC_FMT_DUPIPFLAG", "CA_QC_FMT_DUPIPFLAG",  "CA_STRING", CA_QC_FMT_DUPIPFLAG, 1},
 {"QC_FMT_UNCOMPFLAG", "CA_QC_FMT_UNCOMPFLAG",  "CA_STRING", CA_QC_FMT_UNCOMPFLAG, 1},
 {"QC_DUPPROXYIPFLAG", "CA_QC_DUPPROXYIPFLAG",  "CA_STRING", CA_QC_DUPPROXYIPFLAG, 1},
 {"QC_USELESSIPFLAG", "CA_QC_USELESSIPFLAG",  "CA_STRING", CA_QC_USELESSIPFLAG, 1},
 {"QC_FMT_FIXEDLOOKUP", "CA_QC_FMT_FIXEDLOOKUP",  "CA_STRING", CA_QC_FMT_FIXEDLOOKUP, 1},
 {"QC_USELESSNOREFFLAG", "CA_QC_USELESSNOREFFLAG",  "CA_STRING", CA_QC_USELESSNOREFFLAG, 1},
 {"QC_BADINPUT", "CA_QC_BADINPUT",  "CA_STRING", CA_QC_BADINPUT, 1},
 {"QI_REF_TMOUT", "CA_QI_REF_TMOUT",  "CA_STRING", CA_QI_REF_TMOUT, 1},
 {"QI_REF_OVERMAXLIN", "CA_QI_REF_OVERMAXLIN",  "CA_STRING", CA_QI_REF_OVERMAXLIN, 1},
 {"QI_REF_BADHOST", "CA_QI_REF_BADHOST",  "CA_STRING", CA_QI_REF_BADHOST, 1},
 {"QI_REF_HOSTNOTTRESP", "CA_QI_REF_HOSTNOTTRESP",  "CA_STRING", CA_QI_REF_HOSTNOTTRESP, 1},
 {"QI_FMT_REFHEADER", "CA_QI_FMT_REFHEADER",  "CA_STRING", CA_QI_FMT_REFHEADER, 1},
 {"QI_REFTRAILER", "CA_QI_REFTRAILER",  "CA_STRING", CA_QI_REFTRAILER, 1},
 {"QI_FMT_GROUP_BANNER", "CA_QI_FMT_GROUP_BANNER",  "CA_STRING", CA_QI_FMT_GROUP_BANNER, 1},
 {"QI_FILTER_BANNER", "CA_QI_FILTER_BANNER",  "CA_STRING", CA_QI_FILTER_BANNER, 1},
 {"NS_RIR", "CA_NS_RIR",  "CA_STRING", CA_NS_RIR, 1},
 {"NS_DELEGATIONSFILE", "CA_NS_DELEGATIONSFILE",  "CA_DIRLIST", CA_NS_DELEGATIONSFILE, 1},
 {"NS_SEVERITY", "CA_NS_SEVERITY",  "CA_INT", CA_NS_SEVERITY, 1},
 {"NS_SUFFIX", "CA_NS_SUFFIX",  "CA_STRING", CA_NS_SUFFIX, 1},
 {"NS_NSERVERS", "CA_NS_NSERVERS",  "CA_STRING", CA_NS_NSERVERS, 1},
 {"NS_DELCHECKCONF", "CA_NS_DELCHECKCONF",  "CA_STRING", CA_NS_DELCHECKCONF, 1},


 {"", "", "", CA_NUMBEROFSYMBOLS, 1}
};
#endif

#endif /* CA_DICTIONARY */

