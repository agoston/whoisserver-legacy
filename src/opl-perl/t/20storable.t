
use Test::More qw(no_plan);
use Net::Whois::RIPE::RPSL;
use Data::Dumper;
	
eval {
	use Storable qw(freeze thaw store retrieve);
};

if( $@ ){
	plan skip_all => 'Storable is not available, skipping freeze/thaw tests';
	exit;
}

$test_obj = <<END_OBJECT;
person:       Peter Banik
address:      Singel 258
address:      1016 AB Amsterdam
address:      The Netherlands
phone:        +31 20 5354336
fax-no:       +31 20 5354445
e-mail:       peter\@ripe.net
nic-hdl:      PB836-RIPE
notify:       peter\@ripe.net
changed:      peter\@ripe.net 20030203
source:       RIPE
END_OBJECT


# Test freeze()/thaw()

$obj_orig = Net::Whois::RIPE::RPSL->new($test_obj);
ok( $obj_orig, 'original object created' );
ok( $obj_orig->get('person')->name eq 'person', 'has primary attr');

$obj_frozen = freeze($obj_orig);
ok( $obj_frozen ne '', 'frozen object not empty' );

undef $obj_orig;

$obj_clone = thaw($obj_frozen);
ok( ref($obj_clone) eq 'Net::Whois::RIPE::RPSL', 'got object form thaw()');

ok( $obj_clone->text ne '', 'text() method works');
ok( $obj_clone->get('person')->name eq 'person', 'get attribute works');

# Test store()/retrieve()

$obj_orig_2 = Net::Whois::RIPE::RPSL->new($test_obj);
ok( $obj_orig_2, 'original object created' );
ok( $obj_orig_2->get('person')->name eq 'person', 'has primary attr');


$storable_file = '.storable_test';

$obj_frozen_2 = store($obj_orig_2, $storable_file);
ok( $obj_frozen_2 ne '', 'frozen object not empty' );

ok( -f $storable_file, 'storable file created');


undef $obj_orig_2;

$obj_clone_2 = retrieve($storable_file);
ok( ref($obj_clone_2) eq 'Net::Whois::RIPE::RPSL', 'got object form thaw()');

ok( $obj_clone_2->text ne '', 'text() method works');
ok( $obj_clone_2->get('person')->name eq 'person', 'get attribute works');

ok( unlink($storable_file), 'remove storable file');

