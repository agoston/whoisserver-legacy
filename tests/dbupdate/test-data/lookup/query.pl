#!/usr/bin/perl
use strict;
use warnings;

use DBI;
use File::Spec;
use FindBin;
use Getopt::Long qw( GetOptions );
use IO::Socket::INET6;

use constant EXPECTED_RESULTS => 'lookup';

my $CONFIG = { };

my $SOURCE_DB;

# print a blank line so output is really on a new line
print {*STDERR} "\n";

# How-to
sub usage {
    my $me = $FindBin::Script;

    print
        "Usage: $me <options> -- <query>\n",
        "\n",
        "    Anything before the double-dash (--) is a script option\n",
        "\n",
        "    --ipv4, -4     connect via IPv4 only\n",
        "    --ipv6, -6     connect via IPv6 only\n",
        "\n",
        "    Anything after the double-dash (--) is sent as the lookup query\n"
    ;
    exit;
}

# Process the command line
sub parse_command {
    GetOptions(
        'ipv4|4' => \$CONFIG->{IPV4},
        'ipv6|6' => \$CONFIG->{IPV6},
    ) or usage();

    die "IPv4 and IPv6 options are mutually exclusive"
        if $CONFIG->{IPV4} and $CONFIG->{IPV6};
}

# Parse to locate source.config option
sub locate_source_config_setting {
    my $rip_config = shift;
    
    my $source_config;
    open my $CNF, '<', $rip_config
        or die "Unable to open rip.config: $!";
    while (<$CNF>) {
        if (/^SOURCEFILE\s+([^[:space:]]+)\s*$/) {
            $source_config = $1;
            last;
        }
    }
    close $CNF;
    
    return $source_config;
}

# Parse source config to build database connection table
sub make_database_lookup {
    my $source_config = shift;

    my ($databases, $sources);

    open my $CNF, '<', $source_config
        or die "Unable to open source.config: $!";
    while (my $line = <$CNF>) {
        if ($line =~ /^DATABASE\s+([^#[:space:]]+)/) {
            my $database = $1;

            my @wanted = qw( host port user password);
            my $database_info = { name => $database };

            while ($line = <$CNF>) {
                last if $line =~ m{^/DATABASE};

                my ($varname, $varvalue) = ($line =~ /^\s*(\S+)\s*\:\s*(.*?)\s*$/);

                if (scalar grep { $_ eq $varname } @wanted) {
                    die "source.config database record '$database' has ",
                        "multiple settings for '$varname'"
                            if exists $database_info->{$varname};

                    $database_info->{$varname} = $varvalue;
                }
            }
            
            die "source.config database record '$database' missing settings: ",
                join(',', grep { not exists $database_info->{$_} } @wanted)
                    if scalar grep { not exists $database_info->{$_} } @wanted;

            $database_info->{connstr}
                = sprintf 'dbi:mysql:database=%s;host=%s;port=%d',
                    $database_info->{name},
                    $database_info->{host}, $database_info->{port};

            $databases->{$1} = $database_info;
        } elsif ($line =~ /^SOURCE\s+([^#[:space:]]+)/) {
            my $source = $1;
            
            my $database;
            
            while ($line = <$CNF>) {
                last if $line =~ m{^/SOURCE};
                
                if ($line =~ /^\s*database\s*\:\s*(.*?)\s*$/) {
                    die "source.config source record '$source' has ",
                        "multiple settings for 'database'" if $database;

                    $database = $1;
                }
            }
            
            die "source.config source record '$source' missing settings: database"
                unless $database;
            
            die "source.config source record '$source' is using an unknown database"
                unless $databases->{$database};

            $sources->{$source} = $databases->{$database};
        }
    }
    close $CNF;

    return $sources;
}

# connect, make query, save result
sub attempt_query {
    my $query = shift;

    # check we have the info we need
    my $host = ($CONFIG->{IPV6}
                ? ( $ENV{WHOIS6_HOST} || $ENV{WHOIS_HOST} )
                : $ENV{WHOIS_HOST})
        or die "hostname environment variable not set (WHOIS_HOST &/or WHOIS6_HOST)";

    my $port = $ENV{SVLOOKUP_PORT}
        or die "lookup port environment variable not set (SVLOOKUP_PORT)";

    my $socket = eval {
        IO::Socket::INET6->new(
            Domain      =>  ($CONFIG->{IPV4}
                                ? AF_INET
                                : ($CONFIG->{IPV6}
                                    ? AF_INET6
                                    : AF_UNSPEC)
                            ),
            PeerHost    =>  $host,
            PeerPort    =>  $ENV{SVLOOKUP_PORT},
            Proto       =>  'tcp',
        )
    };
    die "Unable to connect to whois lookup port: ", ($@ || $!) if $@ or not defined $socket;

    # throw the query at the port
    print {$socket} "$query\n";

    # and read the response
    my @lines;
    while (my $line = <$socket>) {
        last if $line eq "\n";
        chomp $line;
        push @lines, $line;
    }

    close $socket;

    return \@lines;
}

sub get_expected_results {
    my $rundir = $ENV{RUNDIR}
        or die "current test directory is not set in environment (RUNDIR)";

    my $result_file = File::Spec->catfile($rundir, EXPECTED_RESULTS);

    open my $ERF, '<', $result_file
        or die "Unable to open expected results file (", EXPECTED_RESULTS, "): $!";

    my @lines;
    while (my $line = <$ERF>) {
        chomp $line;
        push @lines, $line;
    }

    close $ERF;

    return \@lines;
}

sub break_into_objects {
    my $lines = shift;

    my (@objects, $object);

    foreach my $line (@$lines) {
        if ($line =~ /^\s*$/) {
            if (scalar @$object) {
                push @objects, $object;
                undef $object;
            }
        } else {
            push @$object, $line;
        }
    }

    push @objects, $object if scalar @$object;

    return \@objects;
}

sub lines_match {
    my ($seta, $setb) = @_;
    # compare the two arrays of strings
    # return the index of the first mismatching line
    # or return -1 if they match
    # the index returned may not exist in one of the arrays
    # indicating that the other array has more elements

    my $i = 0;
    while ($i < @$seta and $i < @$setb) {
        my $linea = $$seta[$i];
        my $lineb = $$setb[$i];
        
        $linea =~ s/\s+/ /g; # convert tabs and multiple spaces/tabs
        $lineb =~ s/\s+/ /g; #  into one space
        
        $linea =~ s/^\s+//; # trim the lines
        $linea =~ s/\s+$//;
        
        $lineb =~ s/^\s+//;
        $lineb =~ s/\s+$//;

        # and now they should be reasonably good to compare
        # so return the index number if they don't match
        return $i if $linea ne $lineb;

        # next item
        $i++;
    }
    # now if both arrays are of equal size then they can be said to be matching
    # otherwise one array is larger than the other and they don't
    # so if the arrays are the same size return matching (-1), otherwise...
    return (@$seta == @$setb) ? -1 : $i;
}

sub get_object_from_db {
    my ($source, $object_id) = @_;

    # check the source is known
    my $db = $SOURCE_DB->{$source}
        or die "Source '$source' is not a configured source";

    # attempt to connect to the database
    my $dbh = DBI->connect(
        $db->{connstr}, $db->{user}, $db->{password},
        { RaiseError => 0 }
    ) or die "Unable to connect to '$source' database: ", DBI->errstr;

    # attempt to get the object
    my $sth = $dbh->prepare('SELECT object FROM last WHERE object_id=?')
        or die "Unable to prepare sql statement for '$source': ", $dbh->errstr;

    $sth->execute($object_id)
        or die "Unable to get object for '$source': ", $sth->errstr;

    my $row = $sth->fetchrow_arrayref();
    if (not defined $row) {
        if ($sth->err) {
            die "Unable to fetch object for '$source': ", $sth->errstr;
        } else {
            die "Object '$object_id' does not exist in source '$source'";
        }
    }
    die "Fetching object '$object_id' from '$source' resulted in invalid data"
        if ref($row) ne 'ARRAY' or @$row != 1;

    # make sure there are no other objects with the same id
    if ($sth->fetchrow_arrayref() or $sth->err) {
        if ($sth->err) {
            die "A problem occured checking object uniqueness for '$source': ", $sth->errstr;
        } else {
            die "Multiple objects found for object '$object_id' in source '$source'";
        }
    }

    # we have a good object, now to break it into an object array
    my @object = split /\r?\n|\r/, $row->[0];

    return \@object;
}

# MAIN

parse_command();

# Locate rip.config
my $rip_config = $ENV{RIP_CONFIG}
    or die "environment does not contain RIP_CONFIG";

# locate source.config
my $source_config = locate_source_config_setting($rip_config)
    or die "SOURCEFILE setting in rip.config not found";

# gather database connection info
$SOURCE_DB = make_database_lookup($source_config)
    or die "No sources configured";

# send the query to the lookup port
my $results = attempt_query(join(' ', @ARGV));

# did we get a result
die "no lines returned from query"
    if not defined $results or scalar @$results == 0;

# any errors from the lookup
if (scalar grep { $_ =~ /^%/ } @$results) {
    # strip to just the error lines
    @$results = grep { $_ =~ /^%/ } @$results;

    # load the expected results
    my $expected = get_expected_results();

    # check that they match
    my $index = lines_match($results, $expected);
    if ($index >= 0) {
        # oops - no match
        my $result_line = $results->[$index] || '';
        my $expected_line = $expected->[$index] || '';
        die "Error result does not match the expected result:\n",
            "Result:   [$result_line]\nExpected: [$expected_line]\n";
    }
} else {
    # no errors were found in the results
    # so now we have to check for validity

    # load expected results
    my $expected = get_expected_results();
    # break into objects
    my $objects = break_into_objects($expected);

    my $seen_object;

    foreach my $result (@$results) {
        # each line should be a specific format
        my ($source, $object_id, $object_type, $primary_key)
            = ($result =~ /^(\S+),\s(\d+),\s(\S+),\s(.*)$/);
        die "Output is not formatted correctly:\n$result"
            unless $source and $object_id and $object_type and $primary_key;

        # Is it a real object?
        my $db_object = get_object_from_db($source, $object_id);

        # get the real object type and primary key
        my ($real_type, $real_key) = ($db_object->[0] =~ /^(\S+)\:\s+(.*)$/);
        die "Object '$object_id' from '$source' is not formatted properly"
            unless $real_type and $real_key;

        # compare the object type and key with what was reported
        die "Object type does not match reported object type ('$object_id' in '$source')"
            if $real_type ne $object_type;

        $real_key =~ s/^\s+//;
        $real_key =~ s/\s+$//;
        $primary_key =~ s/^\s+//;
        $primary_key =~ s/\s+$//;
        die "Object primary key does not match reported object key ('$object_id' in '$source')"
            if lc($real_key) ne lc($primary_key);
        # the key comparison must be case-insensitive as ipv6 addresses may
        # be lowercase in the object blob, but uppercase in the radix dataleaf

        # now compare the object with the list of expected objects
        # (sorry, brute force for now)
        my $seen = 0;
        my $index = 0;
        while ($index < @$objects) {
            if (lines_match($objects->[$index], $db_object) == -1) {
                $seen = 1;
                $seen_object->[$index] ++;
                last;
            }
            $index++;
        }

        die "Object '$object_id' from '$source' is not an expected object"
            unless $seen;
    }

    # all results have been checked and since we got this far they should be good
    # so now we check that all expected objects were seen
    die "Some expected objects were not seen in the results"
        if (scalar grep { $_ > 0 } @$seen_object) != scalar @$objects;
}

# theoretically we should be done

exit 0;
