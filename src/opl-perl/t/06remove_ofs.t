use Test::More tests => 47;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

use constant ATTRIBUTE_COUNT => 11;

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

# -ive should fail
foreach my $ofs (-1, -2, -50, -1000) {
	ok((not defined $rpsl->remove($ofs)), 
		"remove($ofs): removing a negative offset fails"); 
	ok((defined $rpsl->err->descr), "err:"); 
}

# first attribute should fail
ok((not defined $rpsl->remove(0)), "remove(0): fails"); 
ok((defined $rpsl->err->descr), "err:"); 

# an attribute that doesn't exist should error
ok((not defined $rpsl->remove(1000)), "remove(OFFSET=1000): fails"); 
ok((defined $rpsl->err->descr), "err:"); 

# this seg faults
ok((not defined $rpsl->remove(&ATTRIBUTE_COUNT)),
	"remove(OFFSET=".&ATTRIBUTE_COUNT."): boundary condition"); 


my $ofs = &ATTRIBUTE_COUNT;
while ($ofs-- > 1) {
	is( ref($rpsl->remove($ofs)), 
		'Net::Whois::RIPE::RPSL::Attribute', 
		"remove(OFFSET=$ofs): succeeds"); 
	ok((not defined $rpsl->err), "err: not defined"); 
	ok($rpsl->get_num_attr()==$ofs, "get_num_attr == ".$rpsl->get_num_attr()); 
}

# should be only one attribute left now.
ok($rpsl->get_num_attr()==1, "get_num_attr == 1");
my @attr = $rpsl->get;
ok(@attr==1, "get returns a list of size 1");
ok($attr[0]->name eq 'person', "attribute name is person");
