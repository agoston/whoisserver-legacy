package Net::Whois::RIPE::RPSL::TemplateAttribute;

use strict;
use warnings;
use Net::Whois::RIPE::RPSL;

use Data::Dumper;

our $VERSION=do{my @r=(q$Revision: 1.6 $=~/\d+/g);sprintf "%d."."%02d"x$#r,@r};

=head1 NAME

Net::Whois::RIPE::RPSL::TemplateAttribute - extension for RPSL template attributes

=head1 SYNOPSIS

  use Net::Whois::RIPE::RPSL::TemplateAttribute;

  print $attr->name;            # attribute name
  print $attr->code;            # 2-letters code (pn, so,ch etc)
  print $attr->is_required;       
  print $attr->is_generated;      
  print $attr->is_multivalued;    
  print $attr->is_lookup;         
  print $attr->is_inverse;        
  print $attr->is_primary;        
  print $attr->is_list;           
  print $attr->is_ripe_list;
  print $attr->is_pseudo;       # TRUE if the attribute is
                                # either `delete' or `override'

=head1 DESCRIPTION

This object represents an RPSL template attribute, returned by B<RPSL>::get_template() -- you most 
likely will not need to use it for other purposes.  See the B<RPSL> manpage for details.

=head1 CONSTRUCTORS

=item new( rpsl_attr_tPtr )

=cut

#my @ERRORS;

sub new {
	my $class = shift;
	my $attr = shift || return undef;

	my $self = {};
	if (ref( $attr ) eq 'rpsl_template_tPtr') {
		$self->{ptr} = $attr;
		$self->{destroy} = 0;
	}
	bless $self, $class;
}



=head1 METHODS

=item name

Get the name of the attribute.

=cut

sub name   { Net::Whois::RIPE::RPSL::tmpl_name( $_[0]->{ptr} ) }

=item code

Get code of the attribute.

=cut

sub code   { Net::Whois::RIPE::RPSL::tmpl_code( $_[0]->{ptr} ) }

=item is_required

Returns TRUE if the attribute is required.

=cut

sub is_required	{ Net::Whois::RIPE::RPSL::tmpl_is_required( $_[0]->{ptr} ) }


=item is_generated

Returns TRUE if the attribute is generated.

=cut

sub is_generated	{ Net::Whois::RIPE::RPSL::tmpl_is_generated( $_[0]->{ptr} ) }


=item is_multivalued

Returns TRUE if the attribute is multivalued.

=cut

sub is_multivalued	{ Net::Whois::RIPE::RPSL::tmpl_is_multivalued( $_[0]->{ptr} ) }


=item is_lookup

Returns TRUE if this is a lookup attribute.

=cut

sub is_lookup	{ Net::Whois::RIPE::RPSL::tmpl_is_lookup( $_[0]->{ptr} ) }


=item is_inverse

Returns TRUE for inverse attributes.

=cut

sub is_inverse	{ Net::Whois::RIPE::RPSL::tmpl_is_inverse( $_[0]->{ptr} ) }


=item is_primary

Returns TRUE if the attribute is a primary key.

=cut

sub is_primary	{ Net::Whois::RIPE::RPSL::tmpl_is_primary( $_[0]->{ptr} ) }


=item is_list

Returns TRUE if the attribute is list.

=cut

sub is_list	{ Net::Whois::RIPE::RPSL::tmpl_is_list( $_[0]->{ptr} ) }


=item is_ripe_list

Returns TRUE for RIPE list attributes.

=cut

sub is_ripe_list	{ Net::Whois::RIPE::RPSL::tmpl_is_ripe_list( $_[0]->{ptr} ) }

=item is_pseudo

Returns TRUE if the attribute is either `delete' or `override'.

=cut

sub is_pseudo {
	
	my $self = shift;
	
	if( $self->name eq "delete" || $self->name eq "override" ){
		return 1;
	}
	return 0;
}

1;
__END__

=head1 AUTHOR

Peter Banik <peter@ripe.net>

=head1 SEE ALSO

librpsl
RPSL
Net::Whois::RIPE::RPSL::Attribute
Net::Whois::RIPE::RPSL::Error

=cut



