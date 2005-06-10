package Net::Whois::RIPE::RPSL;

require 5.005_62;
use strict;
use warnings;

use Carp;
require Net::Whois::RIPE::RPSL::Attribute;
require Net::Whois::RIPE::RPSL::Error;
require Net::Whois::RIPE::RPSL::TemplateAttribute;

require Exporter;
require DynaLoader;
use AutoLoader;
use Tie::IxHash;


our @ISA = qw(Exporter DynaLoader);
our $VERSION = '1.16';



our %EXPORT_TAGS = ( 
	'errlvl' => [ qw(
		RPSL_ERRLVL_NONE
		RPSL_ERRLVL_DEBUG
		RPSL_ERRLVL_INFO
		RPSL_ERRLVL_NOTICE
		RPSL_ERRLVL_WARN
		RPSL_ERRLVL_ERROR
		RPSL_ERRLVL_CRIT
		RPSL_ERRLVL_FATAL
		) ],
	'general' => [ qw(
		error_alloc
		get_attr_id
		get_class_id
		load_dictionary
		read_dictionary
		get_classes
		get_template
		) ],
	'object' => [ qw(
		object_add_attr
		object_append_attr
		object_copy
		object_copy_flattened
		object_delete
		object_errors
		object_get_all_attr
		object_get_attr
		object_get_attr_by_ofs
		object_get_class
		object_get_num_attr
		object_get_text
		object_has_error
		object_init
		object_is_deleted
		object_remove_attr
		object_remove_attr_name
		) ],
	'attr' => [ qw(
		attr_clean_copy
		attr_copy
		attr_delete
		attr_delete_list
		attr_errors
		attr_get_clean_value
		attr_get_clean_lines
		attr_get_name
		attr_get_ofs
		attr_get_split_list
		attr_get_value
		attr_has_error
		attr_init
		attr_is_generated
		attr_is_key
		attr_is_lookup
		attr_is_multivalued
		attr_is_required
		attr_replace_value
		) ],
);

our @EXPORT_OK = qw(  );
our @EXPORT = qw( );
Exporter::export_ok_tags('attr');
Exporter::export_ok_tags('errlvl');
Exporter::export_ok_tags('general');

use constant RPSL_STD_COLUMN         => 14;

# various RPSL error levels (similar as syslog errors) 
use constant RPSL_ERRLVL_NONE        =>  0;
use constant RPSL_ERRLVL_DEBUG       =>  1;
use constant RPSL_ERRLVL_INFO        =>  2;
use constant RPSL_ERRLVL_NOTICE      =>  3;
use constant RPSL_ERRLVL_WARN        =>  4;
use constant RPSL_ERRLVL_ERROR       =>  5;
use constant RPSL_ERRLVL_CRIT        =>  6;
use constant RPSL_ERRLVL_FATAL       =>  7;

# error codes
# attribute-related errors 
use constant RPSL_ERR_BADATTR        =>  0;
use constant RPSL_ERR_UNKNOWNATTR    =>  1;
use constant RPSL_ERR_EMPTYLIST      =>  2;
use constant RPSL_ERR_EMPTYATTR      =>  3;
use constant RPSL_ERR_SYNERR         =>  4;
# object-related errors 
use constant RPSL_ERR_ONLYCOMMENTS   =>  5;
use constant RPSL_ERR_BADCLASS       =>  6;
use constant RPSL_ERR_UNKNOWNCLASS   =>  7;
use constant RPSL_ERR_ATTRNOTALLOWED =>  8;
use constant RPSL_ERR_ATTRSINGLE     =>  9;
use constant RPSL_ERR_ATTRGENERATED  => 10;
use constant RPSL_ERR_MISSINGATTR    => 11;
use constant RPSL_ERR_MISSINGKEY     => 12;
# modification-related errors 
use constant RPSL_ERR_BADOFFSET      => 13;
use constant RPSL_ERR_NOSUCHATTR     => 14;

=head1 NAME

RPSL - Perl extension for RPSL

=head1 SYNOPSIS

  use Net::Whois::RIPE::RPSL;

  $person = Net::Whois::RIPE::RPSL->new(<<PERSON_OBJECT);
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
  
  $copy = $person->copy;
  $copy_flat = $person->copy_flattened;

  if ($person->has_error) {
     foreach my $e ($person->errors) {
        print $e->descr, "\n";
	 }
  }

  print $person->text;

  $num = $person->get_num_attr;
  $class_name = $person->class;

  @attributes = $person->get;
  @attributes = $person->get('address');
  $attribute = $person->get($offset);

  $person->add($attribute) or die $person->err->descr;
  $person->add($attribute, $offset) or die $person->err->descr;

  $person->remove('changed') or die $person->err->descr;
  $person->remove($offset) or die $person->err->descr;

  $person->is_required('e-mail');
  $person->is_multivalued('address');
  $person->is_lookup('nic-hdl');
  $person->is_key('nic-hdl');
  $person->is_generated('changed');

  $class_id = Net::Whois::RIPE::RPSL::class_id( CLASS_NAME );

  @class_list = Net::Whois::RIPE::RPSL::get_classes();
  $template = Net::Whois::RIPE::RPSL::get_template("inetnum");

  @attr_names = keys(%$template);

  print $template->{"admin-c"}->code;

  foreach $attr_name (keys %$template){
	$attr = $template->{$attr_name};
  	print $attr->name;
  	print $attr->code;
  	print $attr->is_generated ? "[generated]" : "[not generated]";
  }

=head1 DESCRIPTION

=cut

=head1 CLASS 

=item class_id( CLASS_NAME )

Returns a unique identifier associated with CLASS_NAME. 

=cut

sub class_id {
	shift if $_[0] =~ /^RPSL$/ or ref $_[0] =~ /^RPSL$/;
	return get_class_id( shift );
}

=item get_template( CLASS_NAME )

Returns a reference to a hash (tied to Tie::IxHash in order to prevent the prder of elements) of attributes of the named class.  The hash is indexed by the name of the attribute.  Attributes are represented by a Net::Whois::RIPE::RPSL::TemplateAttribute object.

=cut


sub get_template {

	my $class = shift || return undef;

	my @list = Net::Whois::RIPE::RPSL::get_raw_template($class);
	my %output = ();

	tie(%output, "Tie::IxHash");
	
	map {
		my $attr_t = new Net::Whois::RIPE::RPSL::TemplateAttribute($_);
		my $attr_name = $attr_t->name;
		$output{$attr_name} = $attr_t;
	} @list;

	return \%output;
}


=head1 CONSTRUCTORS

=item new( STRING )

=cut

sub new {
	my $class = shift;
	my $string = shift || die "expected object string not found";

	bless { 
		error => undef, 
		object => object_init( $string ) 
	}, $class;
}

=item copy

Returns a copy of the object passed.  This is a "deep copy",
meaning that all members of the structure are in turn copied.

=cut

sub copy {
	my $self = shift;
	bless { 
		error => undef, 
		object => object_copy( $self->{object} ) 
		}, ref $self;
}

=item copy_flattened

Returns a copy of the object passed.  This is a "deep copy",
meaning that all members of the structure are in turn copied.
Additionally, all attributes are split, in the same way as
rpsl_attr_get_split_list(), and each element added as a seperate
attribute.  For example, an object with:
    
      as-set: AS-XYZ
      members: AS123, AS456

 Will be returned as:

      as-set: AS-XYZ
      members: AS123
      members: AS456

=cut

sub copy_flattened {
	my $self = shift;
	bless { 
		error => undef, 
		object => object_copy_flattened( $self->{object} ) 
		}, ref $self;
}

=head1 METHODS

=item has_error

Returns TRUE if the object has an error.

=cut

sub has_error { object_has_error( $_[0]->{object}, RPSL_ERRLVL_NONE ) }

=item text( [DATA_COLUMN] ) 

Get the data from the object.  If there is none, undef will be
returned. This does not include the trailing '\n' (i.e. a blank
line) that separates RPSL objects.

The optional DATA_COLUMN argument is which column the non-whitespace data of
the object will start at.  You can use 0 for the data_column, which
indicates that no formatting is to be done.  You can also use the
value RPSL_STD_COLUMN, which is 14 (this is the indent used by the
RIPE database). This is the default If no DATA_COLUMN argument is found.

Column numbering starts at 0.  Because attribute names are long, it
does not make sense to use anything shorter than the smallest
attribute name in the object.  If you do, that attribute will have
one space after the ':'.

Attribute values that start before the data_column value will have
enough spaces added before the value so that they start on that
column.  Attribute values that start after the data_column value
will have enough spaces removed so that they start on that column.
Any lines added via line continuation will have that many spaces
added or removed as well.  If data_column is non-zero, then there
will always be at least one space after the ':'.  Non-whitespace
data will never be removed.

Tabstops are considered each 8 characters, meaning when a tab is
encountered it is considered to be enough spaces to advance the
character to the next column position such that (column mod 8) is 0.  
If whitespace is to be removed from a continued line, a tab may be
replaced with the appropriate number of spaces (e.g. Two tabs would
put the data at column 15.  If data_column is 9, the second tab
would be removed and replaced with 2 spaces.)

=cut

sub text { 
	my $self = shift;
	my $col = @_ ? shift : RPSL_STD_COLUMN;

	object_get_text( $self->{object}, $col ) 
}

=item get_num_attr

Return the number of attributes in the object.  For badly-formed
objects, this may be 0.

=cut

sub get_num_attr { 
	my $self = shift;
	object_get_num_attr( $self->{object} ) 
}

=item get( [NAME|OFFSET] )

get returns a list of attributes depending on the call parameters.
If B<no> parameters are found then B<get> returns an in-order list of 
all attributes from the object. 
The first entry will be the class of the object.
If a parameter is passed and it matches a number then it 
will try to retrieve the attribute at that B<OFFSET>. If it doesn't
match a number then it is assumed to be a name and B<get> will
try to retrieve the list of attributes matching B<NAME>. 
An empty list will be returned if no attributes are found.

=cut

sub get { 
	my $self = shift;
	my @pointers;
	if (@_) {
		my $arg = shift;
		@pointers = ($arg =~ /^\d+$/)
			? object_get_attr_by_ofs( $self->{object}, $arg )
			: object_get_attr(        $self->{object}, $arg );
	}
	else {                     # all attributes
		@pointers = object_get_all_attr( $self->{object} ) 
	}

	return unless @pointers;

	return wantarray 
		?  map { $_ = Net::Whois::RIPE::RPSL::Attribute->new($_) } @pointers
		:  Net::Whois::RIPE::RPSL::Attribute->new($pointers[0]);
}

=item add( ATTRIBUTE [, OFFSET] )

Add the specified ATTRIBUTE to the object before OFFSET specified.
On success, 1 is returned.  
On failure, 0 is returned, and the error structure is filled with details 
about the reason (e.g. attribute not permitted in this class).  
query B<err> for the B<Net::Whois::RIPE::RPSL::Error> structure. If no OFFSET is found then 
the ATTRIBUTE is appended to the end of the object.  
Unlike attributes added by B<new>, there will be no errors added to the 
class itself if an attribute with an error is added via this function.
Also unlike B<new>, a "generated" attribute may be added without error 
with this function.
Note the first attribute (offset 0) is intentionally prevented from
being replaced.

=cut

sub add { 
	my $self = shift;
	my $attr = shift;
	my $error = Net::Whois::RIPE::RPSL::Error->new;


	# to stop people adding the same pointer twice
	my $copy = attr_copy( $attr->{ptr} );

	my $rc = @_
		? object_add_attr( $self->{object}, $copy, shift, $error )
		: object_append_attr( $self->{object}, $copy, $error );

	if ($rc) {
		$self->{error} = undef;
	}
	else {
		attr_delete( $copy ); # loose the copy
		$self->{error} = $error;
	}

	return $rc;
}

sub add2 { 
	my $self = shift;
	my $attr = shift;
	my $error = Net::Whois::RIPE::RPSL::Error->new;

	# here we add it

	my $rc = @_
		? object_add_attr(    $self->{object}, $attr->{ptr}, shift, $error )
		: object_append_attr( $self->{object}, $attr->{ptr}, $error );

	if ($rc) {
		$attr->_destroy(0);     # $self->{object} takes care of deletion
		$self->{error} = undef;
	}
	else {
		$self->{error} = $error;
	}

	return $rc;
}

=item remove( NAME|OFFSET )

Remove the attribute at the specified OFFSET or with the specified NAME from 
the object.
On success, 1 is returned.
On failure, 0 is returned, query B<err> for the B<Net::Whois::RIPE::RPSL::Error> structure 
with details about the reason (e.g. removing mandatory attribute). 
Note the first attribute (offset 0) is intentionally prevented from 
being removed.

=cut

sub remove { 
	my $self = shift;
	my $arg = shift;

	$self->{error} = Net::Whois::RIPE::RPSL::Error->new;

	my $ptr = ($arg =~ /^\d+$/)
		? object_remove_attr(      $self->{object}, $arg, $self->{error} )
		: object_remove_attr_name( $self->{object}, $arg, $self->{error} );

	return unless $ptr;
	$self->{error} = undef;

	my $attr = Net::Whois::RIPE::RPSL::Attribute->new($ptr);
	# we have to manage the memory for these suckers.
	$attr->_destroy(1);
	return $attr;
}

=item err

Returns the B<Net::Whois::RIPE::RPSL::Error> object associated with the last call
to B<add> or B<remove>. If no error occured then undef is returned.
Don't confuse this with B<errors> which returns the list of errors
on the object.

=cut

sub err       { return $_[0]->{error} }

=item class

Returns the name of the class of the object.  If there is none
(only possible for an invalid class), then undef will be returned.

=cut

sub class { object_get_class( $_[0]->{object} ) }

=item errors 

Get a list of errors for this object. This list may be empty.  

=cut

sub errors { object_errors( $_[0]->{object} ) }

=item is_required( ATTRIBUTE_NAME )

returns TRUE if ATTRIBUTE_NAME is a required attribute for this object.

=cut

sub is_required    { attr_is_required( $_[0]->{object}, $_[1] ) }

=item is_multivalued( ATTRIBUTE_NAME )

returns TRUE if ATTRIBUTE_NAME is multivalued attribute for this object.

=cut

sub is_multivalued { attr_is_multivalued( $_[0]->{object}, $_[1] ) }

=item is_lookup( ATTRIBUTE_NAME )

returns TRUE if ATTRIBUTE_NAME is a lookup attribute for this object.

=cut

sub is_lookup      { attr_is_lookup( $_[0]->{object}, $_[1] ) }

=item is_key( ATTRIBUTE_NAME )

returns TRUE if ATTRIBUTE_NAME is a key attribute for this object.

=cut

sub is_key         { attr_is_key( $_[0]->{object}, $_[1] ) }

=item is_generated( ATTRIBUTE_NAME )

returns TRUE if ATTRIBUTE_NAME is a generated attribute for this object.

=cut

sub is_generated   { attr_is_generated( $_[0]->{object}, $_[1] ) }


=item STORABLE_freeze

Hook for Storable.

=cut

sub STORABLE_freeze {
	my $self = shift;
	my $clone = shift;
	return $self->text;
}

=item STORABLE_thaw

Hook for Storable.

=cut

sub STORABLE_thaw {
	my $self = shift;
	my $clone = shift;
	my $serialized_data = shift;

	unless( $serialized_data ){
		warn "WARNING: Net::Whois::RIPE::RPSL->STORABLE_thaw()) called but got no serialized data.\n";
		return;
	}

	# FIXME
	# duplicate code copied from new()
	# This stuff should really be moved out to
	# a separate method and let that be called from here and from
	# new() as well.
	#
	# I'll wait with that until I figure out a better implementation
	# for the Storable hooks.
	# peter 20030220
	#
	$self->{error} = undef;
	$self->{object} = object_init( $serialized_data );
	return $self;
}


sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.  If a constant is not found then control is passed
    # to the AUTOLOAD in AutoLoader.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "& not defined" if $constname eq 'constant';
    my $val = constant($constname, @_ ? $_[0] : 0);
    if ($! != 0) {
	if ($! =~ /Invalid/ || $!{EINVAL}) {
	    $AutoLoader::AUTOLOAD = $AUTOLOAD;
	    goto &AutoLoader::AUTOLOAD;
	}
	else {
	    croak "Your vendor has not defined RPSL macro $constname";
	}
    }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
	if ($] >= 5.00561) {
	    *$AUTOLOAD = sub () { $val };
	}
	else {
	    *$AUTOLOAD = sub { $val };
	}
    }
    goto &$AUTOLOAD;
}

bootstrap Net::Whois::RIPE::RPSL $VERSION;

=item get_classes

returns a list of the implemented classes.

=cut

1;
__END__

=head1 AUTHOR

K. Baker <kevin@ripe.net>

=head1 CAVEATS

=item adding attributes to an object with B<add> so that an invalid
object becomes valid won't be reported correctly by B<has_error> and
B<errors>. These methods assume the stance they took when the object
was first created.

=item calling B<remove>(OFFSET) with an offset that is one greater than the
last offset causes a seg fault.

=item B<add> makes a copy of the Net::Whois::RIPE::RPSL::Attribute and adds the copy to the
object. This may bite if you think that altering an attribute you thought
you added will alter the attribute in the object. It won't. This may change

=item B<XS>. rpsl_object_get_attr_by_ofs spits out a warning. This is because
it is 'const rpsl_attr_t' as the return value but I dropped the 'const' so
that perl would bless it as type 'rpsl_attr_tPtr' rather than 
'const rpsl_attr_tPtr'. This as ramifications for Net::Whois::RIPE::RPSL::Attribute since it
checks the ref of objects in it's constructor.

=head1 TODO

=item new(<<MANY_OBJECTS)

Allow new to take multiple objects separated by double newlines and return
an array of objects. Or allow it to accept a file handle? Filename...

=head1 SEE ALSO

librpsl
Net::Whois::RIPE::RPSL::Attribute
Net::Whois::RIPE::RPSL::Error
Net::Whois::RIPE::RPSL::TemplateAttribute

=cut
