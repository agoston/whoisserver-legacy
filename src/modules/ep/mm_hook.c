#include "mm_internal.h"
//stolen from mtest in imap-2001a
void mm_searched (MAILSTREAM *stream, unsigned long number)
{
}


void mm_exists (MAILSTREAM *stream,unsigned long number)
{
}


void mm_expunged (MAILSTREAM *stream,unsigned long number)
{
}


void mm_flags (MAILSTREAM *stream,unsigned long number)
{
}


void mm_notify (MAILSTREAM *stream,char *string,long errflg)
{
  mm_log (string,errflg);
}


void mm_list (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
{
  putchar (' ');
  if (delimiter) putchar (delimiter);
  else fputs ("NIL",stdout);
  putchar (' ');
  fputs (mailbox,stdout);
  if (attributes & LATT_NOINFERIORS) fputs (", no inferiors",stdout);
  if (attributes & LATT_NOSELECT) fputs (", no select",stdout);
  if (attributes & LATT_MARKED) fputs (", marked",stdout);
  if (attributes & LATT_UNMARKED) fputs (", unmarked",stdout);
  putchar ('\n');
}

void mm_lsub (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
{
  putchar (' ');
  if (delimiter) putchar (delimiter);
  else fputs ("NIL",stdout);
  putchar (' ');
  fputs (mailbox,stdout);
  if (attributes & LATT_NOINFERIORS) fputs (", no inferiors",stdout);
  if (attributes & LATT_NOSELECT) fputs (", no select",stdout);
  if (attributes & LATT_MARKED) fputs (", marked",stdout);
  if (attributes & LATT_UNMARKED) fputs (", unmarked",stdout);
  putchar ('\n');
}


void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
{
  printf (" Mailbox %s",mailbox);
  if (status->flags & SA_MESSAGES) printf (", %lu messages",status->messages);
  if (status->flags & SA_RECENT) printf (", %lu recent",status->recent);
  if (status->flags & SA_UNSEEN) printf (", %lu unseen",status->unseen);
  if (status->flags & SA_UIDVALIDITY) printf (", %lu UID validity",
                                              status->uidvalidity);
  if (status->flags & SA_UIDNEXT) printf (", %lu next UID",status->uidnext);
  printf ("\n");
}


void mm_log (char *string,long errflg)
{
/*
  switch ((short) errflg) {
  case NIL:
    printf ("[%s]\n",string);
    break;
  case PARSE:
  case WARN:
    printf ("%%%s\n",string);
    break;
  case ERROR:
    printf ("?%s\n",string);
    break;
  }
*/
}


void mm_dlog (char *string)
{
  puts (string);
}

void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
{
}


void mm_critical (MAILSTREAM *stream)
{
}


void mm_nocritical (MAILSTREAM *stream)
{
}


long mm_diskerror (MAILSTREAM *stream,long errcode,long serious)
{
  kill (getpid (),SIGSTOP);
  return NIL;
}


void mm_fatal (char *string)
{
  printf ("?%s\n",string);
}

