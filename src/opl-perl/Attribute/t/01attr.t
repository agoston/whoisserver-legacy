use Test::More tests => 24;
use strict;

BEGIN { use_ok('Net::Whois::RIPE::RPSL::Attribute') }


# make an Net::Whois::RIPE::RPSL::Attribute, specifiying the class

my $attr = Net::Whois::RIPE::RPSL::Attribute->new('person:       Fred Bloggs ', 'person');

ok(ref($attr) eq 'Net::Whois::RIPE::RPSL::Attribute',            "new: ref is Net::Whois::RIPE::RPSL::Attribute ".ref($attr));
ok($attr->has_error==0,                   "has_error: is false");
my @err = $attr->errors;
ok(@err==0,                               "errors: returns empty list");
ok(my $attr_copy = $attr->copy,           "copy: is defined ");
ok($attr->name eq 'person',               "name: eq 'person'");
ok($attr->value eq '       Fred Bloggs ', "value: matches");

# make an Net::Whois::RIPE::RPSL::Attribute, no class, should realise from the string

$attr = Net::Whois::RIPE::RPSL::Attribute->new('person:       Fred Bloggs ');

ok(ref($attr) eq 'Net::Whois::RIPE::RPSL::Attribute',            "new: ref is Net::Whois::RPSL::Attribute");
ok($attr->has_error==0,                   "has_error: is false");
ok($attr->name eq 'person',               "name: eq 'person'");
ok($attr->value eq '       Fred Bloggs ', "value: matches");

$attr->replace_value('        Shag Malone'); 
ok($attr->has_error==0, "has_error: after replace_value returns 0");
ok($attr->value eq '        Shag Malone', "value: matches replaced value");
ok((not defined $attr->offset),           "offset: is undefined");

# clean_value , should remove newline and surrounding whitespace

$attr = Net::Whois::RIPE::RPSL::Attribute->new(<<PERSON_ATTR, 'person');
person:  Fred Bloggs
PERSON_ATTR
ok($attr->has_error==0,                   "has_error: is false");
ok($attr->name eq 'person',               "name: eq 'person'");
ok($attr->clean_value eq 'Fred Bloggs',   "clean_value: matches");
ok((not defined $attr->offset),           "offset: is undefined");

# no value, test 1: no space after the colon 

ok($attr = Net::Whois::RIPE::RPSL::Attribute->new('person:', 'person'),"new: created");
ok($attr->has_error==1,                   "has_error: is true");
ok(@err = $attr->errors,               "errors: returns a list");

# no value, test 2: space after the colon

ok($attr = Net::Whois::RIPE::RPSL::Attribute->new('person: ', 'person'),"new: created");
ok($attr->has_error==1,                   "has_error: is true");
ok(@err = $attr->errors,               "errors: returns a list");
