use Test::More tests => 9;

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

# get( NAME )           
# aka rpsl_get_attr

ok((my @person = $rpsl->get('person'))==1, "get(person): ok");
ok((my @address = $rpsl->get('address'))==3, "get(address): ok");
ok((my @phone = $rpsl->get('phone'))==1, "get(phone): ok");
ok((my @fax = $rpsl->get('fax-no'))==1, "get(fax-no): ok");
ok((my @email = $rpsl->get('e-mail'))==1, "get(e-mail): ok");
ok((my @nichdl = $rpsl->get('nic-hdl'))==1, "get(nic-hdl): ok");
ok((my @changed = $rpsl->get('changed'))==2, "get(changed): ok");
ok((my @source = $rpsl->get('source'))==1, "get(source): ok");
