package Net::Whois::RIPE::RPSL::Error;

use strict;
use warnings;
require Net::Whois::RIPE::RPSL; # for XS implementation

our $VERSION=do{my @r=(q$Revision: 1.3 $=~/\d+/g);sprintf "%d."."%02d"x$#r,@r};

=head1 NAME

Net::Whois::RIPE::RPSL::Error - extension for RPSL Errors 

=head1 SYNOPSIS

  use Net::Whois::RIPE::RPSL::Error;

  $err->level          # level of the error.  use Net::Whois::RIPE::RPSL qw(:errlvl)
  $err->code           # code for the error
  $err->descr          # description of the error
  $err->attr_num       # index of attribute with error

=head1 DESCRIPTION

Implemented in RPSL.xs. Used by RPSL and Net::Whois::RIPE::RPSL::Attribute to provide
a perl interface to the C structure for rpsl_error_t. 

=head1 CONSTRUCTOR

=item new( [rpsl_error_tPtr] )

Creates a new error object. If the optional parameter B<rpsl_error_tPtr>
is passed then this struture is copied.

=cut

1;
__END__

=head1 AUTHOR

K. Baker, <kevin@ripe.net>

=head1 TODO

=item B<level> and B<code> may not serve much purpose. The error string
may be sufficient for the perl interface. In this case Net::Whois::RIPE::RPSL::Error can
dissappear and RPSL and Net::Whois::RIPE::RPSL::Attribute can simply return strings.

=head1 SEE ALSO

RPSL
Net::Whois::RIPE::RPSL::Attribute

=cut
