use Test::More tests => 8;

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

ok(ref($rpsl) eq 'Net::Whois::RIPE::RPSL', "new: ref is RPSL");
ok(my $rpsl_copy = $rpsl->copy, "copy: is defined ");
ok(my $rpsl_copy_flat = $rpsl->copy_flattened, "copy_flattened: is defined ");

# check copy 
ok($rpsl_copy->text eq $rpsl->text, "text: copy matches original");

# check class
ok($rpsl->class eq 'person', "class: matches 'person'");
ok($rpsl_copy->class eq 'person', "class: copy matches 'person'");
ok($rpsl_copy_flat->class eq 'person', "class: copy_flat matches 'person'");
