use Test::More tests => 11;
$|++;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

use Net::Whois::RIPE::RPSL::Attribute;

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
source:       RIPE
PERSON_OBJECT

## add success

ok($rpsl->get_num_attr==10,'get_num_attr=10');
ok(my $attr=Net::Whois::RIPE::RPSL::Attribute->new('changed:      hostmaster@nic.uk 20000404'),
	'Net::Whois::RIPE::RPSL::Attribute->new');

# add before primary attribute will always fail
ok($rpsl->add($attr, 0)==0,'add($attr, 0): should fail');

# find the offset of changed + 1. 
# this is where we are going to add

my $ofs = 0;
my @changed=$rpsl->get('changed');
foreach (@changed) { $ofs = $_->offset if $_->offset > $ofs }
ok($ofs > 0, "found the offset of the last 'changed'");
$ofs++; # so that it is after the last changed

ok($rpsl->add($attr, $ofs)==1,'add:');
ok($rpsl->add($attr, $ofs)==1,'add:');
ok($rpsl->add($attr, $ofs)==1,'add:');
ok($rpsl->add($attr, $ofs)==1,'add:');
ok($rpsl->add($attr, $ofs)==1,'add:');
ok($rpsl->get_num_attr==15,'get_num_attr==15 ');
