use Test::More tests => 19;
use strict;
use warnings;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

#	address
#	changed
my @NAMES = (qw/
	phone
	fax-no
	e-mail
	nic-hdl
	source
	/);

my $rpsl = Net::Whois::RIPE::RPSL->new(<<PERSON_OBJECT);
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


# first attribute should fail
ok((not defined $rpsl->remove('person')), "remove(person): fails"); 

# remove names single attr

foreach my $name (@NAMES) {
	my $attr1= $rpsl->remove($name);
	my $attr2 = $rpsl->remove($name);
	is(ref($attr1), 'Net::Whois::RIPE::RPSL::Attribute',  "remove($name): succeeds"); 
	is((not defined $attr2),      1,    "remove($name): fails"); 
}

# multi
my $add1 = $rpsl->remove('address');
my $add2 = $rpsl->remove('address');
my $add3 = $rpsl->remove('address');
my $add4 = $rpsl->remove('address');
is(ref($add1), 'Net::Whois::RIPE::RPSL::Attribute',  "remove(address): address 1 ok"); 
is(ref($add2), 'Net::Whois::RIPE::RPSL::Attribute',  "remove(address): address 2 ok"); 
is(ref($add3), 'Net::Whois::RIPE::RPSL::Attribute',  "remove(address): address 3 ok"); 
is((not defined $add4),      1,    "remove(address): address 4 time fails"); 

my $chg1 = $rpsl->remove('changed');
my $chg2 = $rpsl->remove('changed');
my $chg3 = $rpsl->remove('changed');
is(ref($chg1), 'Net::Whois::RIPE::RPSL::Attribute',  "remove(changed): changed 1 ok"); 
is(ref($chg2), 'Net::Whois::RIPE::RPSL::Attribute',  "remove(changed): changed 2 ok"); 
is((not defined $chg3),       1,   "remove(changed): changed 3 time fails"); 
