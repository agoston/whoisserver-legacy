#!/usr/bin/perl -w

use strict;

use YAML::XS;

use Test::More 'no_plan';

require_ok 'Net::Whois::RIPE::RPSL';

my $rpsl = Net::Whois::RIPE::RPSL->new(<<'PERSON_OBJECT');
person:       Fred Bloggs 
address:      38 Branfiddle Lane
address:      Corset
address:      England
phone:        +23 04 4444444
fax-no:       +23 04 4534534
e-mail:       bloggs@fred.uk
nic-hdl:      FB934-RIPE
changed:      hostmaster@fred.uk 19990427
changed:      hostmaster@nic.uk 20000404
source:       RIPE
PERSON_OBJECT

isa_ok $rpsl, 'Net::Whois::RIPE::RPSL';
is_deeply Load($rpsl->yaml_dump), Load(<<'YAML'),       "yaml_dump, no errors";
attributes:
  - person:       Fred Bloggs 
  - address:      38 Branfiddle Lane
  - address:      Corset
  - address:      England
  - phone:        +23 04 4444444
  - fax-no:       +23 04 4534534
  - e-mail:       bloggs@fred.uk
  - nic-hdl:      FB934-RIPE
  - changed:      hostmaster@fred.uk 19990427
  - changed:      hostmaster@nic.uk 20000404
  - source:       RIPE  
class:
  name:         person
  key:          FB934-RIPE
errors: []
YAML
