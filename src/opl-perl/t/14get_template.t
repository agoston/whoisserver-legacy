use Test::More tests => 3;

BEGIN { use_ok('Net::Whois::RIPE::RPSL') }

is( Net::Whois::RIPE::RPSL::get_template(), undef );

my @classes = Net::Whois::RIPE::RPSL::get_classes();

my $tests_total = $#classes + 1;
my $tests_performed = 0;

foreach my $test_class (@classes){

  my $test_template = Net::Whois::RIPE::RPSL::get_template($test_class);
  ++$tests_performed if (scalar(keys %$test_template));
  print "$tests_performed $test_class\n";
}

ok( $tests_performed == $tests_total );

