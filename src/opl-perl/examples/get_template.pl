#!/usr/bin/perl -w

use strict;
use Net::Whois::RIPE::RPSL;

my @class_list = Net::Whois::RIPE::RPSL::get_classes();

foreach my $class (@class_list){

	print "** Class $class\n";
	print "-"x44 . "\n";
	my $template = Net::Whois::RIPE::RPSL::get_template($class);

	foreach my $attr_name (keys %$template){

		my $attr = $template->{$attr_name};

		printf("\%-20s \%-4s \%d \%d \%d \%d \%d \%d \%d \%d \%d\n", 
			$attr->name,
			$attr->code, 
			$attr->is_required, 
			$attr->is_generated, 
			$attr->is_multivalued, 
			$attr->is_lookup, 
			$attr->is_inverse, 
			$attr->is_primary, 
			$attr->is_list, 
			$attr->is_ripe_list, 
			$attr->is_pseudo
		);
	}

	print "\n";

}
