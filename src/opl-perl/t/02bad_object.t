use Test::More tests => 4;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }


my $rpsl = Net::Whois::RIPE::RPSL->new(<<PERSON_OBJECT);
person:       Fred Bloggs 
a ddress:      38 Branfiddle Lane
a ddress:      Corset
a ddress:      England
p hone:        +23 04 4444444
f ax-no:       +23 04 4534534
e -mail:       bloggs\@fred.uk
n ic-hdl:      FB934-RIPE
c hanged:      hostmaster\@fred.uk 19990427
changed:      hostmaster\@nic.uk 20 000404
s ource:       RIPE
PERSON_OBJECT

ok(ref($rpsl) eq 'Net::Whois::RIPE::RPSL', "new: ref is RPSL");

ok($rpsl->has_error, "has_error: returns true");

ok(my @errors = $rpsl->errors,"errors: is defined");
#foreach my $e (@errors) { printf STDERR STDERR "%s\n", $e->descr; }
