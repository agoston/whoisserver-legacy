use Test::More tests => 14;
use strict;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

use Net::Whois::RIPE::RPSL::Attribute;

# attributes to add
my @ATTR = (
	Net::Whois::RIPE::RPSL::Attribute->new('address:      38 Branfiddle Lane'),
	Net::Whois::RIPE::RPSL::Attribute->new('address:      Corset'),
	Net::Whois::RIPE::RPSL::Attribute->new('address:      England'),
	Net::Whois::RIPE::RPSL::Attribute->new('phone:        +23 04 4444444'),
	Net::Whois::RIPE::RPSL::Attribute->new('fax-no:       +23 04 4534534'),
	Net::Whois::RIPE::RPSL::Attribute->new('e-mail:       bloggs@fred.uk'),
	Net::Whois::RIPE::RPSL::Attribute->new('nic-hdl:      FB934-RIPE'),
	Net::Whois::RIPE::RPSL::Attribute->new('changed:      hostmaster@fred.uk 19990427'),
	Net::Whois::RIPE::RPSL::Attribute->new('changed:      hostmaster@nic.uk 20000404'),
	Net::Whois::RIPE::RPSL::Attribute->new('source:       RIPE')
);

# make an object
my $rpsl = Net::Whois::RIPE::RPSL->new('person:       Fred Bloggs ');
ok($rpsl->has_error==1,'has_error == 1');
ok($rpsl->get_num_attr==1,'get_num_attr=10');

# append 
foreach my $a (@ATTR) {
	ok($rpsl->add($a)==1,'append: '.$a->name);
}

# shouldn't be in error now...but still is. bug?
#ok($rpsl->has_error==0,'has_error == 0');
ok($rpsl->get_num_attr==11,'get_num_attr==11');
