use Test::More tests => 6;
use strict;

BEGIN { use_ok('Net::Whois::RIPE::RPSL::Attribute') }


my $attr = Net::Whois::RIPE::RPSL::Attribute->new(<<MEMBERS,'as-set');
members:  AS123, AS456,AS333
MEMBERS

ok((defined $attr), "new: succeeds");

my @list = $attr->split;

ok(@list == 3, "split: returns a list 3 element big");

ok($list[0]->value eq 'AS123', "list[0]->value: matches 'AS123'");
ok($list[1]->value eq 'AS456', "list[1]->value: matches 'AS456'");
ok($list[2]->value eq 'AS333', "list[2]->value: matches 'AS333'");
