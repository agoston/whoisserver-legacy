package Net::Whois::RIPE::RPSL::Attribute;

use strict;
use warnings;
use Net::Whois::RIPE::RPSL;

our $VERSION=do{my @r=(q$Revision: 1.3 $=~/\d+/g);sprintf "%d."."%02d"x$#r,@r};

=head1 NAME

Net::Whois::RIPE::RPSL::Attribute - extension for RPSL attributes

=head1 SYNOPSIS

  use Net::Whois::RIPE::RPSL::Attribute;

  $attr_id = Net::Whois::RIPE::RPSL::Attribute::attr_id( ATTRIBUTE_NAME )

  $attr = Net::Whois::RIPE::RPSL::Attribute->new('person:   Fred Bloggs');
  $attr = Net::Whois::RIPE::RPSL::Attribute->new('person:   Fred Bloggs ', 'person');
  $copy = $attr->copy;

  if ($attr->has_error) {
  	foreach $e ($attr->errors) { print $e->descr, "\n" }
	exit;
  }

  print $attr->name;         # returns 'person'
  print $attr->value;        # returns '   Fred Bloggs '
  print $attr->clean_value;  # returns 'Fred Bloggs'
  print $attr->offset;       # returns offset with RPSL object

  $attr->replace_value('   Geoff Bloggs'); 

  @attributes = $attr->split;


=head1 DESCRIPTION

RPSL attributes.

=head1 CONSTRUCTORS

=item new( STRING, CLASS )

Creates an attribute with the given STRING of attribute class CLASS.
The string specified by the CLASS parameter may either be undef, or
the name of the class that this attribute is for.  This is necessary
for attributes like "member-of", whose syntax varies depending on
which class it is in.  If the syntax is always the same (e.g.
"changed"), or is only specified for a single class (e.g.
"referral-by"), then either undef or the class name to be used may be
passed safely.   If you specify a class name and the attribute is
not in that class, this is an error and undef will be returned.  If
the syntax is ambiguous without a class context (e.g. "member-of")
and you do not specify the class, this is also an error, and undef 
will be returned.
On success, a new B<Net::Whois::RIPE::RPSL::Attribute> is returned, otherwise undef 
is returned (possible as described above).  Syntax errors may be
found by querying at the errors method of the returned object.

=item new( rpsl_attr_tPtr )

Create a new Net::Whois::RIPE::RPSL::Attribute using an existing B<rpsl_attr_tPtr>.
Ugly but true. This second form exists due to the relationship
between perl and librpsl. Sometimes we need to create Net::Whois::RIPE::RPSL::Attribute
objects for attributes returned from RPSL. In that case librpsl
takes care of freeing the memory. This method exists to wrap 
those attributes so they are still accesible via this interface. 
This form should only ever need to be called by B<RPSL> methods.

=cut

#my @ERRORS;

sub new {
	my $class = shift;
	my $attr_str = shift || die "expected attribute string not found";
	my $attr_class = @_ ? shift : undef;

	my $self = {};
	if (ref( $attr_str ) eq 'rpsl_attr_tPtr') {
		# already existing, 
		$self->{ptr} = $attr_str;
		$self->{destroy} = 0;
	}
	else {
		no warnings;
		$self->{ptr} = Net::Whois::RIPE::RPSL::attr_init( $attr_str, $attr_class );
		use warnings;
		return undef unless $self->{ptr};
		$self->{destroy} = 1;
	}

	bless $self, $class;
}

=item copy

Returns a copy of the attribute passed.  This is a "deep copy",
meaning that all members of the structure are in turn copied.

=cut

sub copy {
	my $self = shift;
	bless { 
		destroy => 1, 
		ptr => Net::Whois::RIPE::RPSL::attr_copy( $self->{ptr} ) 
	}, ref $self;
}

=head1 CLASS METHODS

=item attr_id( ATTRIBUTE_NAME )

Returns a unique identifier associated with ATTRIBUTE_NAME.
(These also match the attributes used by the definition files
generated for the RIP application.)

=cut

sub attr_id {
	shift if $_[0] =~ /^RPSL$/ or ref $_[0] =~ /^RPSL$/;
	return get_attr_id( shift );
}

=head1 METHODS

=item has_error

Returns TRUE if the attribute has an error. Query B<errors> to get
at the errors.

=cut

sub has_error { 
	my $self = shift;
	my $errlvl = shift || &Net::Whois::RIPE::RPSL::RPSL_ERRLVL_NONE;
	Net::Whois::RIPE::RPSL::attr_has_error( $self->{ptr}, $errlvl );
}

=item name

Get the name of the attribute.  If the attribute has no name
(possible for badly-formatted attributes, or ones with invalid
names), then undef will be returned.

=cut

sub name   { Net::Whois::RIPE::RPSL::attr_get_name( $_[0]->{ptr} ) }

=item value

Get the data from the attribute.  If no value was found, then undef 
will be returned.

=cut

sub value  { Net::Whois::RIPE::RPSL::attr_get_value( $_[0]->{ptr} ) }

=item clean_value

Get a cleaned copy of the data from the attribute.  Any end-of-line
comments will be removed, continued lines joined together,
runs of whitespace converted to a single space, and leading and
trailing whitespace removed.  If no value was found, then undef will
be returned.

=cut

sub clean_value  { Net::Whois::RIPE::RPSL::attr_get_clean_value( $_[0]->{ptr} ) }

=item clean_lines

Get a cleaned copy of the data from the attribute.  Any end-of-line
comments or line continuation characters will be removed, runs of
whitespace converted to a single space, and leading and trailing
whitespace on each line will be removed.  If no value was found,
then NULL will be returned.

The memory for this copy is allocated by malloc(), and must be freed
by the caller.

=cut

sub clean_lines  { Net::Whois::RIPE::RPSL::attr_get_clean_lines( $_[0]->{ptr} ) }

=item replace_value( STRING )

Replace the current value of the attribute with the parameter
passed.  An attempt to replace the value with an illegal value will 
succeed, and the error will be added to the error list for the
attribute.

=cut

sub replace_value  { 
	my ($self, $value) = @_;
	# only permit replace_value on Attributes that we manage mem for
	return 0 if $self->{destroy} == 0;
	Net::Whois::RIPE::RPSL::attr_replace_value( $self->{ptr}, $value );
}

=item offset 

Returns the offset of this attribute within the B<RPSL> object that
contains it, or undef if it is not part of an object.

=cut

sub offset { 
	my $ofs = Net::Whois::RIPE::RPSL::attr_get_ofs( $_[0]->{ptr} );
	return ($ofs == -1 ? undef : $ofs)
}

=item split 

Split attribute lists into separate elements, and return a list of
these.  If the attribute is not a list type, then a list with a
single element is returned. If no value is found, then an empty
list will be returned.

=cut

sub split { 
	my $self = shift;
	my @ret;
	foreach my $attr ( Net::Whois::RIPE::RPSL::attr_get_split_list( $self->{ptr} ) ) {
		push @ret, bless { destroy => 1, ptr => $attr }, ref $self;
	}
	@ret
}

=item errors

Get a list of errors for this attribute. This list may be empty.
Errors are objects of type B<Net::Whois::RIPE::RPSL::Error>. 

=cut

sub errors { Net::Whois::RIPE::RPSL::attr_errors( $_[0]->{ptr} ) }

=head2 Internal

=item _destroy( [BOOLEAN] )

Get/Set the internal flag that determines whether this object will ask
the C library to free the C structure. 

=cut

sub _destroy {
	my $self = shift;
	my $val = $self->{destroy};
	$self->{destroy} = shift if @_;
	$val;
}


sub DESTROY {
	my $self = shift;
	# only attr_delete the attr if we created it
	Net::Whois::RIPE::RPSL::attr_delete( $self->{ptr} ) if $self->{destroy};
}


1;
__END__
=head1 AUTHOR

K. Baker, <kevin@ripe.net>

=head1 SEE ALSO

perl(1).

=cut
