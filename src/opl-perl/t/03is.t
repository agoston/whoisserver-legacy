use Test::More tests => 41;

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

ok($rpsl->is_required('person')==1, "is_required(person): true");
ok($rpsl->is_required('address')==1, "is_required(address): true");
ok($rpsl->is_required('phone')==1, "is_required(phone): true");
ok($rpsl->is_required('fax-no')==0, "is_required(fax-no): is false");
ok($rpsl->is_required('e-mail')==0, "is_required(e-mail): is false");
ok($rpsl->is_required('nic-hdl')==1, "is_required(nic-hdl): true");
ok($rpsl->is_required('changed')==1, "is_required(changed): true");
ok($rpsl->is_required('source')==1, "is_required(source): true");

ok($rpsl->is_multivalued('person')==0, "is_multivalued(person): is false");
ok($rpsl->is_multivalued('address')==1, "is_multivalued(address): true");
ok($rpsl->is_multivalued('phone')==1, "is_multivalued(phone): true");
ok($rpsl->is_multivalued('fax-no')==1, "is_multivalued(fax-no): true");
ok($rpsl->is_multivalued('e-mail')==1, "is_multivalued(e-mail): true");
ok($rpsl->is_multivalued('nic-hdl')==0, "is_multivalued(nic-hdl): is false");
ok($rpsl->is_multivalued('changed')==1, "is_multivalued(changed): true");
ok($rpsl->is_multivalued('source')==0, "is_multivalued(source): is false");

ok($rpsl->is_lookup('person')==1, "is_lookup(person): true");
ok($rpsl->is_lookup('address')==0, "is_lookup(address): is false");
ok($rpsl->is_lookup('phone')==0, "is_lookup(phone): is false");
ok($rpsl->is_lookup('fax-no')==0, "is_lookup(fax-no): is false");
ok($rpsl->is_lookup('e-mail')==1, "is_lookup(e-mail): true");
ok($rpsl->is_lookup('nic-hdl')==1, "is_lookup(nic-hdl): true");
ok($rpsl->is_lookup('changed')==0, "is_lookup(changed): is false");
ok($rpsl->is_lookup('source')==0, "is_lookup(source): is false");

ok($rpsl->is_key('person')==0, "is_key(person): is false");
ok($rpsl->is_key('address')==0, "is_key(address): is false");
ok($rpsl->is_key('phone')==0, "is_key(phone): is false");
ok($rpsl->is_key('fax-no')==0, "is_key(fax-no): is false");
ok($rpsl->is_key('e-mail')==0, "is_key(e-mail): is false");
ok($rpsl->is_key('nic-hdl')==1, "is_key(nic-hdl): true");
ok($rpsl->is_key('changed')==0, "is_key(changed): is false");
ok($rpsl->is_key('source')==0, "is_key(source): is false");

ok($rpsl->is_generated('person')==0, "is_generated(person): is false");
ok($rpsl->is_generated('address')==0, "is_generated(address): is false");
ok($rpsl->is_generated('phone')==0, "is_generated(phone): is false");
ok($rpsl->is_generated('fax-no')==0, "is_generated(fax-no): is false");
ok($rpsl->is_generated('e-mail')==0, "is_generated(e-mail): is false");
ok($rpsl->is_generated('nic-hdl')==0, "is_generated(nic-hdl): is false");
ok($rpsl->is_generated('changed')==0, "is_generated(changed): is false");
ok($rpsl->is_generated('source')==0, "is_generated(source): is false");
