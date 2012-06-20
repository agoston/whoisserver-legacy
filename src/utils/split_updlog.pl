#!/usr/bin/perl
#
# feed legacy dbupdate's updlog on its stdin, will produce /tmp/upd%08d from them
# agoston, 2012-06-20

use strict;
use warnings;

my $nextIsMail = 0;
my $count = 1;
my @contents = ();

while (<>) {
  my $line = $_;
  if (/^>>> time: (?:[A-Z][a-z][a-z] ){2}[0-9]+ (?:[0-9]{2}:){2}[0-9]{2} [0-9]{4} (MAIL |SYNC |)UPDATE .*<<<$/) {

    my $isMail = $nextIsMail;
    $nextIsMail = ($1 eq "MAIL ");

    if ($#contents > 5 && $isMail) {
      open FILE, sprintf('>/tmp/upd%08d', $count);
      # pop the starting newline
      shift @contents;
      # pop the endling newline
      pop @contents;
      # now write
      foreach (@contents) {
        print FILE $_;
      }
      close FILE;
      $count++;
    }
    undef @contents;
  } else {
    push @contents, $line;
  }
}
