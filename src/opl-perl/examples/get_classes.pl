#!/usr/bin/perl -w

use strict;
use Net::Whois::RIPE::RPSL;

my @class_list = Net::Whois::RIPE::RPSL::get_classes(); 

foreach my $class (@class_list){
	print "Class: $class\n";
}


