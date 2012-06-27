#!/usr/bin/perl
#
# feed legacy dbupdate's updlog on its stdin, will produce /tmp/upd%08d from them
# agoston, 2012-06-20

use strict;
use warnings;

my $count = 1;

local $/ = ">>> time: ";
while (<>) {
    s{$/$}{}; # Strip separator

    my ($header, undef, @body) = split /\n/;

    next unless $header;
    next unless $header =~ m{MAIL UPDATE};
    next unless @body > 5;

    open my $fh, '>', sprintf '/tmp/upd%08d', $count++;
    print $fh join "\n", @body, '';
    close $fh;
}
