use Test::More tests => 2;
use strict;

require "t/mem.pl";

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }


# look for memory leaks in add 

my $DEBUG      = 0;             # set to 1 for debug messages
my $ITERATIONS = 10_000;        # 1_000_000;
my $HALFWAY    = $ITERATIONS/2; # integer please
my $TOLERANCE  = 4;             # max difference (in bytes) between two_iter and halfway/last


## main


my $basemem = mem();
my $one_iter = 0;
my $two_iter = 0;
my $halfway = 0;


my $i=0;
while ( $i++ < $ITERATIONS ) {
	my $rpsl = Net::Whois::RIPE::RPSL->new(person());

	my $attr = Net::Whois::RIPE::RPSL::Attribute->new('address:      blah blah');
	foreach (1..10) {
		$rpsl->remove('address');
		$rpsl->remove('changed');
	}

	$one_iter = mem() if $i == 1;
	$two_iter = mem() if $i == 2;
	$halfway = mem() if $i == $HALFWAY;
}
my $lastmem = mem();

# for some reason one_iter is always a few bytes short
# of the others. I'm happy if it doesn't budge over 
# 100,000 iterations from the 2nd iteration
ok((
     ($halfway - $two_iter) <= $TOLERANCE and 
     ($lastmem - $two_iter) <= $TOLERANCE
	), "leak test");

if ($DEBUG) {
	printf STDERR "iterations    %7d\n", $ITERATIONS;
	printf STDERR "base mem        %5d bytes\n", $basemem;
	printf STDERR "one iteration   %5d bytes\n", $one_iter;
	printf STDERR "two iterations  %5d bytes\n", $two_iter;
	printf STDERR "halfway         %5d bytes\n", $halfway;
	printf STDERR "last mem        %5d bytes\n", $lastmem;
}

exit;

sub person {
return <<PERSON_OBJECT;
person:       Fred Bloggs 
address:      38 Branfiddle Lane
address:      Corset
address:      England
phone:        +23 04 4444444
fax-no:       +23 04 4534534
e-mail:       bloggs\@fred.uk
nic-hdl:      FB934-RIPE
changed:      hostmaster\@fred.uk 19990427
changed:      hostmaster\@nic.uk 20000404
source:       RIPE
PERSON_OBJECT
}
