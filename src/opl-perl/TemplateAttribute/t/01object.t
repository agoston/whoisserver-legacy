use Test::More tests => 2;

BEGIN { use_ok('Net::Whois::RIPE::RPSL::TemplateAttribute') }

my $ta = Net::Whois::RIPE::RPSL::TemplateAttribute->new();

# check for failed instantiation if no valid pointer was passed to constructor
is ( $a, undef );

undef $a;
