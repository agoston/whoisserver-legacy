use Test::More tests => 34;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

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

# get( OFFSET )           
# aka rpsl_get_attr_by_ofs

# by array

ok((my @person   = $rpsl->get(0))==1, "get(0): ok");
ok((my @address1 = $rpsl->get(1))==1, "get(1): ok");
ok((my @address2 = $rpsl->get(2))==1, "get(2): ok");
ok((my @address3 = $rpsl->get(3))==1, "get(3): ok");
ok((my @phone    = $rpsl->get(4))==1, "get(4): ok");
ok((my @fax      = $rpsl->get(5))==1, "get(5): ok");
ok((my @email    = $rpsl->get(6))==1, "get(6): ok");
ok((my @nichdl   = $rpsl->get(7))==1, "get(7): ok");
ok((my @changed1 = $rpsl->get(8))==1, "get(8): ok");
ok((my @changed2 = $rpsl->get(9))==1, "get(9): ok");
ok((my @source   = $rpsl->get(10))==1, "get(10): ok");

# by scalar

ok(($rpsl->get(0)->name eq 'person'), "get(0)->name: eq 'person");
ok(($rpsl->get(0)->value eq '       Fred Bloggs '), "get(0)->value:");
ok(($rpsl->get(1)->name eq 'address'));
ok(($rpsl->get(1)->value eq '      38 Branfiddle Lane'));
ok(($rpsl->get(2)->name eq 'address'));
ok(($rpsl->get(2)->value eq '      Corset'));
ok(($rpsl->get(3)->name eq 'address'));
ok(($rpsl->get(3)->value eq '      England'));
ok(($rpsl->get(4)->name eq 'phone'));
ok(($rpsl->get(4)->value eq '        +23 04 4444444'));
ok(($rpsl->get(5)->name eq 'fax-no'));
ok(($rpsl->get(5)->value eq '       +23 04 4534534'));
ok(($rpsl->get(6)->name eq 'e-mail'));
ok(($rpsl->get(6)->value eq '       bloggs@fred.uk'));
ok(($rpsl->get(7)->name eq 'nic-hdl'));
ok(($rpsl->get(7)->value eq '      FB934-RIPE'));
ok(($rpsl->get(8)->name eq 'changed'));
ok(($rpsl->get(8)->value eq '      hostmaster@fred.uk 19990427'));
ok(($rpsl->get(9)->name eq 'changed'),'get(9)->name');
ok(($rpsl->get(9)->value eq '      hostmaster@nic.uk 20000404'));
ok(($rpsl->get(10)->name eq 'source'),'get(10)->name');
ok(($rpsl->get(10)->value eq '       RIPE'));
