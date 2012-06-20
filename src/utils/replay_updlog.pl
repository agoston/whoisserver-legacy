#!/usr/bin/perl

use strict;
use warnings;

open FILE, ">/tmp/here" || die 'Cannot open /tmp/here for writing';

my $nextmflag = "";
my $keywords = "";

while (<>) {
  my $line = $_;
  if (/^>>> time: (?:[A-Z][a-z][a-z] ){2}[0-9]+ (?:[0-9]{2}:){2}[0-9]{2} [0-9]{4} (MAIL |SYNC |)UPDATE .*<<<$/) {
    close FILE;

    my $mflag = $nextmflag;
    if ($1 eq "MAIL ") {
      $nextmflag = "-m";
    } else {
      $nextmflag = "";
    }

    if ($keywords eq "None") {
      $keywords = "";
    }

    if ($1 eq "MAIL ") {
      system("/export/services/whois-update/bin/dbupdate -r -c /export/services/whois-update/conf/rip.config -f /tmp/here ${mflag}");
    } elsif ($1 eq "SYNC ") {
      system("/export/services/whois-update/bin/dbupdate -r -c /export/services/whois-update/conf/rip.config -f /tmp/here ${keywords}");
    } else {
      system("/export/services/whois-update/bin/dbupdate -r -c /export/services/whois-update/conf/rip.config -f /tmp/here ${keywords}");
    }
    
    $keywords = "";

    open FILE, ">/tmp/here";
  } else {
    if (length($keywords) == 0 && /^keywords - (.*)$/) {
      $keywords = '-k '.$1;
    }
    print FILE $line;
  }
}
