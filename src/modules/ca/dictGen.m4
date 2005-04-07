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
define(`ADAPT', ` divert(9) dnl
{"$1", "CA_$1",  "CA_$2", CA_$1, $3},
divert(8) dnl
 CA_$1,
divert')

#ifndef CA_DICTIONARY
#define CA_DICTIONARY

#define CAP_W 0x000001
#define CAP_U 0x000002



ADAPT(TESTMODE, BOOLEAN, 1)
ADAPT(OVERRIDECRYPTEDPW, STRING, 1)
ADAPT(OVERRIDEPGP, STRING, 1)
ADAPT(QRYLOG, DIRLIST, 1)
ADAPT(DBUPERR, DIRLIST, 1)
ADAPT(ALLRIPERR, DIRLIST, 1)
ADAPT(RIPAUDIT, DIRLIST, 1)
ADAPT(RIPMIRLOG, DIRLIST, 1)
ADAPT(RIPSVRLOG, DIRLIST, 1)
ADAPT(SQLOG, DIRLIST, 1)
ADAPT(RIPUPDLOG, DIRLIST, 1)
ADAPT(UPDLOG, DIRLIST, 1)
ADAPT(ACKLOG, DIRLIST, 1)
ADAPT(ACKTEMPLATE, DIRLIST, 1)
ADAPT(NOTIFTEMPLATE, DIRLIST, 1)
ADAPT(FRWDTEMPLATE, DIRLIST, 1)
ADAPT(FORWCREATETEMPLATE, DIRLIST, 1)
ADAPT(LOCKDIR, DIRLIST, 1)
ADAPT(UPDATELOCK, STRING, 1)
ADAPT(KEEPOPEN, INT, 1)
ADAPT(ALLOCMNT, STRING, 1)
ADAPT(ORG_POWER_MNT, STRING, 1)
ADAPT(TMPDIR, DIRLIST, 1)
ADAPT(RIR, STRING, 1)
ADAPT(SAWFILE, DIRLIST, 1)
ADAPT(GPGCMD, DIRLIST, 1)
ADAPT(OPENSSLCMD, DIRLIST, 1)
ADAPT(CRLLIST, DIRLIST, 1)
ADAPT(PGPPATH, DIRLIST, 1)
ADAPT(CACERTS, DIRLIST, 1)
ADAPT(MAILCMD, DIRLIST, 1)
ADAPT(HUMAILBOX, STRING, 1)
ADAPT(AUTOBOX, STRING, 1)
ADAPT(DEFMAIL, STRING, 1)
ADAPT(AUTHMETHOD, STRING, 1)
ADAPT(REFERRALTIMEOUT, INT, 1)
ADAPT(REFERRALMAXLINES, INT, 1)
ADAPT(NICSUFFIX, STRING, 1)
ADAPT(COUNTRY, STRING, 1)
ADAPT(RIPADMIN, ADMIN, 1)
ADAPT(NOTIFLOG, DIRLIST, 1)
ADAPT(FORWLOG, DIRLIST, 1)
ADAPT(UPDSOURCE, UPDSOURCE, 1)
ADAPT(SVWHOIS_PORT, INT, 1)
ADAPT(SVCONFIG_PORT, INT, 1)
ADAPT(SVMIRROR_PORT, INT, 1)
ADAPT(SOURCEFILE, DIRLIST, 1)
ADAPT(AC_DECAY_INTERVAL, INT, 1)
ADAPT(AC_DECAY_HALFLIFE, INT, 1)
ADAPT(AC_LOAD, BOOLEAN, 1) 
ADAPT(AC_AUTO_SAVE, BOOLEAN, 1)
ADAPT(AC_SAVE_INTERVAL, INT, 1)
ADAPT(PW_RESP_HEADER, STRING, 1)
ADAPT(PW_ERR_NOKEY, STRING, 1)
ADAPT(PW_HELP_FILE, DIRLIST, 1)
ADAPT(PW_K_FILTER, STRING, 1)
ADAPT(PW_FMT_LIMIT_REACHED, STRING, 1)
ADAPT(PW_FMT_ACL_ADDRPASS, STRING, 1)
ADAPT(PW_FMT_ACL_PERMDENY, STRING, 1)
ADAPT(PW_FMT_ACL_CONNDENY, STRING, 1)
ADAPT(PW_NOTFOUND, STRING, 1)
ADAPT(PW_CONNCLOSED, STRING, 1)
ADAPT(PW_BANNER, STRING, 1)
ADAPT(PW_ERR_LINETOOLONG, STRING, 1)
ADAPT(QC_FMT_ATTRNOTINV, STRING, 1)
ADAPT(QC_FMT_BADSOURCE, STRING, 1)
ADAPT(QC_FMT_BADATTR, STRING, 1)
ADAPT(QC_FMT_BADOBJTYPE, STRING, 1)
ADAPT(QC_FMT_DUPIPFLAG, STRING, 1)
ADAPT(QC_DUPPROXYIPFLAG, STRING, 1)
ADAPT(QC_USELESSIPFLAG, STRING, 1)
ADAPT(QC_FMT_FIXEDLOOKUP, STRING, 1)
ADAPT(QC_FMT_UNCOMPFLAG, STRING, 1)
ADAPT(QC_USELESSNOREFFLAG, STRING, 1)
ADAPT(QC_BADINPUT, STRING, 1)
ADAPT(QI_REF_TMOUT, STRING, 1)
ADAPT(QI_REF_OVERMAXLIN, STRING, 1)
ADAPT(QI_REF_BADHOST, STRING, 1)
ADAPT(QI_REF_HOSTNOTTRESP, STRING, 1)
ADAPT(QI_FMT_REFHEADER, STRING, 1)
ADAPT(QI_REFTRAILER, STRING, 1)
ADAPT(NS_RIR, STRING, 1)
ADAPT(NS_DELEGATIONSFILE, DIRLIST, 1)
ADAPT(NS_SEVERITY, INT, 1)
ADAPT(NS_SUFFIX, STRING, 1)
ADAPT(NS_NSERVERS, STRING, 1)
ADAPT(NS_DELCHECKCONF, STRING, 1)

typedef enum
{
undivert(8)

  CA_NUMBEROFSYMBOLS

} ca_dictsym;
 
#ifndef DICT_INIT
extern dict_t dictionary[];
#else 
dict_t dictionary[] = 
{
undivert(9)

 {"", "", "", CA_NUMBEROFSYMBOLS, 1}
};
#endif

#endif /* CA_DICTIONARY */

