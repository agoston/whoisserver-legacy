use Test::More tests => 3;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }


ok(my @classes = Net::Whois::RIPE::RPSL::get_classes(), "get_classes: is defined ");
ok($#classes);
