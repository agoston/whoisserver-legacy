use Test::More tests => 40;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }


use constant ATTR_COUNT => 11;

my @EXPECTED = (
	['person',     '       Fred Bloggs '],     # note the extra white space!
	['address',    '      38 Branfiddle Lane'],
	['address',    '      Corset'],
	['address',    '      England'],
	['phone',      '        +23 04 4444444'],
	['fax-no',     '       +23 04 4534534'],
	['e-mail',     '       bloggs@fred.uk'],
	['nic-hdl',    '      FB934-RIPE'],
	['changed',    '      hostmaster@fred.uk 19990427'],
	['changed',    '      hostmaster@nic.uk 20000404'],
	['source',     '       RIPE'],
	);


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


ok($rpsl->has_error==0,"has_error: is false");
ok(my @attr = $rpsl->get,"get: returns list");
ok(@attr == &ATTR_COUNT,"get: matches expected size");

foreach my $i (0..&ATTR_COUNT-1) {
	ok($attr[$i]->offset == $i, "attr offset matches $i");
	ok($attr[$i]->name eq $EXPECTED[$i][0], 
		"attr name matches '$EXPECTED[$i][0]'");
	ok($attr[$i]->value eq $EXPECTED[$i][1], 
		"attr value match '$EXPECTED[$i][1]'");
}

# by scalar

ok($first_attr = $rpsl->get,"get: returns 1st attribute in scalar context");
ok($first_attr->name eq 'person',"matches person");
ok($first_attr->offset == 0,"offset is 0");
