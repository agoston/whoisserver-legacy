use Test::More tests => 2;
use strict;
BEGIN { use_ok('Net::Whois::RIPE::RPSL::Attribute') }

require "t/mem.pl";

# look for memory leaks in Net::Whois::RIPE::RPSL::Attribute->split


my $DEBUG = 1;               # set to 1 for debug messages
my $ITERATIONS = 10_000;     # 1_000_000;
my $HALFWAY = $ITERATIONS/2; # integer please
my $TOLERANCE  = 4;          # max difference (in bytes) between two_iter and halfway/last


## main

my $basemem = mem();
my $one_iter = 0;
my $two_iter = 0;
my $halfway = 0;


my $i=0;
while ( $i++ < $ITERATIONS ) {
	use Net::Whois::RIPE::RPSL::Attribute;

	# create an attribute, allocs mem
	my $attr = Net::Whois::RIPE::RPSL::Attribute->new('person:       Fred Bloggs ', 'person');
	# check split
	my @splita = $attr->split;
	my @splitb = $attr->split;
	my @splitc = $attr->split;
	my @splitd = $attr->split;

	# take some memory usage snapshots
	$one_iter = mem() if $i == 1;
	$two_iter = mem() if $i == 2;
	$halfway = mem() if $i == $HALFWAY;
}
my $lastmem = mem();

if ($DEBUG) {
	printf "iterations    %7d\n", $ITERATIONS;
	printf "base mem        %5d bytes\n", $basemem;
	printf "one iteration   %5d bytes\n", $one_iter;
	printf "two iterations  %5d bytes\n", $two_iter;
	printf "halfway         %5d bytes\n", $halfway;
	printf "last mem        %5d bytes\n", $lastmem;
}

# for some reason one_iter is always a few bytes short
# of the others. I'm happy if it doesn't budge over 
# 100,000 iterations from the 2nd iteration
#ok(($halfway == $two_iter and $two_iter == $lastmem), "leak test");
ok((($lastmem - $halfway) <= $TOLERANCE), "leak test");


