use Test::More tests => 6;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

use Net::Whois::RIPE::RPSL;

ok((my $err = new Net::Whois::RIPE::RPSL::Error), "RPSL::Error->new");
ok((not defined $err->descr), "Net::Whois::RIPE::RPSL::Error->descr not defined");
ok($err->attr_num==0, "Net::Whois::RIPE::RPSL::Error->attr_num == 0");
ok($err->code==0, "Net::Whois::RIPE::RPSL::Error->code == 0");
ok($err->level==0, "Net::Whois::RIPE::RPSL::Error->level == 0");

