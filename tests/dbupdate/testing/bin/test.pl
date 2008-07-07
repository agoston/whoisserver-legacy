#!/usr/bin/perl -w 
#
#########################################
#  test.pl
#
#  Status: NOT REVIEWED, TESTED
#
#  Author(s):       Katie Petrusha
#
#  ######################################
#  Modification History:
#        katie (07/07/2002) Created.
#	 katie (30/07/2002) Zero-prototype done.
#	 katie (23/08/2002) 1-prototype done, tested with Perl v5.6.0.
#  ######################################
#  Copyright (c) 2001                              RIPE NCC
#
#  All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and its
#  documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and that
#  both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
#  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
#  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
#  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
#  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
########################################

use strict;
use POSIX qw(strftime);
use Getopt::Long;

use IPC::Open3;
use IO::Select;
use IO::Socket;
use IO::Handle;
use IO::Socket::INET6;

# global configuration
our $CONFIG = {};

# general error formats
our $ERR = {
    'E_SINGATTR'  => "ERROR: name should be single: %s\n",
    'E_INVREGEXP' => "ERROR: is not valid regular expression: %s\n",
    'E_INVATTR'   => "ERROR: unknown attribute %s\n",
    'E_SMALLOBJ'  => "ERROR: at least two attributes must be specified\n",
    'E_INVFLTR'   =>
        "ERROR: filter name cannot be resolved: %s in object: %s\n",
    'E_INVOBJ'  => "ERROR: unknown object type: %s\n",
    'E_NOENTRY' =>
        "ERROR: no entry in whois output file for query: %s of object %s\n",
    'E_CONFVAR' =>
        "ERROR: mandatory variable missing in configuration file: ",
    'E_COMMRUN'  => "ERROR: when running %s: %s\n",
    'E_DIE'      => "ERROR: program died when running %s\n",
    'E_COMMOBJ'  => "ERROR: when running %s: expected: %d submitted: %d \n",
    'E_DUPFLTR'  => "ERROR: filter already exists: %s\n",
    'E_FOPEN'    => "ERROR: cannot open %s : %s \n",
    'E_FCLOSE'   => "ERROR: cannot close %s : %s \n",
    'E_TIME'     => "ERROR: operation timeout: %s\n",
    'E_REFUSE'   => "ERROR: connection refused for: %s\n",
    'E_COMLINE'  => "ERROR: in the command line\n",
    'E_LIMIT'    => "ERROR: limit of mismatches exceeded: %d\n",
    'E_DIR'      => "ERROR: no such directory: %s\n",
    'E_UNDEF'    => "ERROR: the object is not defined: %s\n",
    'E_NOOUTPUT' => "ERROR: no output for the object %s found in file %s\n",
    'E_WHOIS'    => "ERROR: connection to whois failed\n",
    'E_DBUPDATE' =>
        "ERROR: in dbupdate logging indentation: line %d function %s\n",
    'E_DBUPDATE_LOG' =>
        "ERROR: line %d: confused about function names\n%s should be %s\nindentation of %d\n"
};

# Global variable to keep track if we have received or our children received
# an INT signal.  In such a case, we want to peacefully exit.
my $received_interrupt = 0;

# global array and hash for signal names and numbers. See get_signal_numbers
# subroutine.
my %signo;
my @signame;

##############################
# Function: getvar($)
# Descr: access variable from config (global configuration hash)
# Args: $name        name of the variable to look up
# Return: returns value or undef if variable not defined.
##############################
sub getvar($) {
    my $name = $_[0];

    if ( exists $CONFIG->{$name} ) {
        return ( $CONFIG->{$name} );
    }
    return undef;
}

##############################
# Function: setvar($)
# Descr: assign variable in config
# Args: $name           variable name
#       $val            assign this value
# Return: always returns 1
##############################
sub setvar($$) {
    my $name = $_[0];
    my $val  = $_[1];

    $CONFIG->{$name} = $val;
#print " ********************************************** SET: ".$name." = ".$val."\n";
    return (1);

}
##############################
# Function: delvar($)
# Descr: delete variable from config
# Args: $name           variable name
# Return: always returns 1
##############################
sub delvar($) {
    my $name = $_[0];

    delete $CONFIG->{$name};
    #print " ********************************************** DEL: ".$name."\n";
    return (1);

}
##############################
# Function: is_valid_pattern($)
# Descr:  check if entered regexp is valid
# Args: $pat           string containing regexp
# Return: returns 1 if ok, 0 if failed
sub is_valid_pattern($) {
    my $pat = $_[0];

    return eval { "" =~ /$pat/; 1 } || 0;

}

##############################
# Function: parse_generic($)
# Descr: parses generic entries in "attr: value" form from filters.config file
# Args:  $FILE           pointer to the file to read from
#        $start          start to parse the file from this string
#        $logstring      contains all types of regexps available.
#                        this is taken from <type>LOG secuences
#                        in configuration file.
# Return: returns the %hash containing the entry or dies if error
# Output: maybe some error messages
# Remarks: syntax check is made to make sure that name attribute is unique
sub parse_generic($$$) {

    my $FILE      = $_[0];
    my $start     = $_[1];    # where to start object parsing mode
    my $logstring = $_[2];

    # reference to a hash
    my $generic = {};
    my $string;

    $string = $start;
    while ( $string && ( $string !~ /^[\s]*$/o ) ) {

        if ( $string =~ /^(#)|(%)/o ) {
            # eat up comments inside the object
            $string = <$FILE>;
            next;
        }
        my $val;
        my $attr;
        if ( $string =~ /^(.+?):[\s]*(.+?)[\s]*$/o ) {
            $attr = $1;
            $val  = $2;
        }
        else {
            error( 'E_INVATTR', $string );
        }
        if (   ( $attr eq "object" )
            || ( $attr eq "filter" )
            || ( $attr eq "lines" ) )
        {
            if ( $attr eq "object" ) {
                if ( $val =~ /^([a-z0-9-]+)[\s]*(.*?)$/oi ) {
                    $val = $1;
                    $val .= " " . $2 if ($2);
                }
            }
            # objects names are single
            if ( !exists $generic->{$attr} ) {
                $generic->{$attr} = $val;
            }
            else {
                error( 'E_SINGATTR', $attr );
            }
        }
        elsif (( $attr eq "whois" )
            && ( $val !~ /^[\s]*?!|DATE_ON|DATE_OFF[\s]*?(EXACT)?$/oi ) )
        {
            error( 'E_INVATTR', $string );

        }
        elsif ( ( $logstring =~ /$attr/ ) && ( is_valid_pattern($val) == 0 ) )
        {
            error( 'E_INVREGEXP', $val );
        }
        else {
            push @{ $generic->{$attr} }, $val;
        }
        # read next line
        $string = <$FILE>;
    }

    return ($generic);

}

##############################
# Function: syntax_check($$)
# Descr: syntactically checks entry based on pattern hash
# Input: $entry          reference to a hash containing the entry,
#                        in form attribute=>value.
#        $pattern        reference to a hash containing the names
#                        of the attributes allowed in this object,
#                        in form name=>""
# Return: 1 if OK, undef if failed.
# Output: error messages
# Remarks: syntax check is made that name attribute is unique
sub syntax_check($$) {
    my $entry   = $_[0];
    my $pattern = $_[1];

    for ( keys %{$entry} ) {
        if ( !exists $pattern->{$_} && ( $_ !~ /^\$[0-9]{1}$/ ) ) {
            error( 'E_INVATTR', $_ );
        }
    }
    if ( scalar( keys %{$entry} ) < 2 ) {
        error('E_SMALLOBJ');
    }
    # default
    if ( ( exists $entry->{'object'} ) && ( !exists $entry->{'lines'} ) ) {
        $entry->{'lines'} = 'multiple';
    }
    return (1);

}

##############################
# Function: reference_check($$)
# Descr: check if all filters mentioned in "body" attrs are mentioned in %filters hash
# Input: @objects         array to store the references to hashes.
#                         Every hash represents the object in
#                         attribute=>value form
#        $filters         reference to the hash containing filter names
#                         represented as name=>""
#
# Return: dies if check failed , 1 if ok
# Output: maybe some error messages
# Remarks:
sub reference_check($$) {
    # ref to array
    my $objects = $_[0];
    # ref to hash
    my $filters = $_[1];
    my $entry;

    foreach $entry ( @{$objects} ) {
        # $entry is a object hash
        my %hash = %{$entry};
        for my $key ( keys %hash ) {
            if ( $key eq "body" ) {
                my @array = @{ $hash{$key} };
                foreach (@array) {
                    my $tmp = $_;
                    $tmp =~ s/[\s]*\(.*\)[\s]*//;
                    if ( !exists $filters->{$tmp} ) {
                        error( 'E_INVFLTR', $tmp, $hash{"object"} );
                    }
                }
            }
        }
    }
    return (1);
}
##############################
# Function: parse_filters()
# Descr: parses file; extracts objects/filters and puts the to hashes/arrays, checks everything
# Input: $filter_file          configuration file name
# Return: dies with a message if failed; returns reference to hash of objects/filters if ok.
# Output: error messages
# Remarks:
sub parse_filters($) {
    my $filter_file = $_[0];

    # ref to array
    my $objects = [];

    # ref to hash
    my $filters = {};

    # construct filter pattern from LOGSTRING config variable

    my @fp_as_array = split( '[\s]+', getvar('LOGSTRING') );
    my %filter_pattern;
    foreach (@fp_as_array) {
        $filter_pattern{$_} = "";
    }
    $filter_pattern{'whois'} = "";
    $filter_pattern{'query'} = "";

    # filter attr
    $filter_pattern{"filter"} = "";

    my %object_pattern
        = ( 'object', '', 'body', '', 'whois', '', 'lines', '', 'query', '' );
    my $data = [];

    # die if filter file not found - fatal for global file
    open( FILTER_FILE, "< $filter_file" )
        or error( 'E_FOPEN', $filter_file, $! );

    while (<FILTER_FILE>) {
        my $str = $_;
        if (/^[a-z0-9-\$]+:/) {
            # ref to hash
            my $entry
                = parse_generic( *FILTER_FILE, $str, getvar('LOGSTRING') );

            if ( exists $entry->{"object"} ) {
                push @{$objects}, $entry;
                syntax_check( $entry, \%object_pattern );
            }
            elsif ( exists $entry->{"filter"} ) {
                if ( exists $filters->{ $entry->{"filter"} } ) {
                    error( "E_DUPFLTR", $entry->{"filter"} );
                }
                $filters->{ $entry->{"filter"} } = $entry;
                syntax_check( $entry, \%filter_pattern );
            }
            else {
                error( "E_INVATTR", $str );
            }
        }
        elsif (/^(%.*)?[\s]?$/og) {
            # skip comments/blank lines
            next;
        }
        elsif (/^#/o) {
            # these comments go into reportfile
            report($_);
        }
        else {
            error( 'E_INVATTR', $_ );
        }
    }
    push @{$data}, $objects, $filters;
    return ($data);
}

##############################
# Function: is_negative()
# Descr: to determine if the regular expression is negative or positive (contains '!' in front or not
# Input: $regex                a string containing regular expression
# Return: 1 if negative, undef if not
# Output: nothing
# Remarks: if regexp has "^![\s]...  it is negative
sub is_negative($) {
    my $regex = $_[0];

    if ( $regex =~ /^!/o ) {
        return (1);
    }
    return (undef);
}

##############################
# Function: is_empty()
# Descr: to determine if the regular expression is empty and negative (contains only '!' ans spaces)
# Input: $regex                a string containing regular expression
# Return: 1 if empty, undef if not
# Output: nothing
# Remarks: if regexp has "^![\s]*$  it is negative
sub is_empty($) {
    my $regex = $_[0];

    if ( $regex =~ /^![\s]*$/o ) {
        return (1);
    }
    return (undef);

}

##############################
## Function check_dbupdate($)
#
#
sub check_dbupdate($) {
    my $tmpfile = shift;

    unless ( -e $tmpfile ) {
        #report ("NO FILE [$tmpfile]\n");
        return;
    }

    # report ("checked [$tmpfile]\n");
    open( TMPFILE, "< $tmpfile" )
        or error( 'E_FOPEN', $tmpfile );

    # xxx: this code is from Shane

    my @function_stack;
    my $line_num = 0;
    while (<TMPFILE>) {
        $line_num++;
        if (/^(\s+)(\S+): enter(ed|ing)/) {
            push( @function_stack, [ $2, $1 ] );
        }
        elsif (/^(\s+)(\S+): exiting/) {
            my $info = pop(@function_stack);
            my ( $fname, $spaces ) = @{$info};
            if ( $fname eq $2 ) {
                unless ( $spaces eq $1 ) {
                    error( 'E_DBUPDATE', $line_num, $fname );
                }
            }
            else {
                error( 'E_DBUPDATE_LOG', $line_num, $2, $fname,
                    ( 0 + @function_stack ) );
            }
        }
    }
    close(TMPFILE);
    unlink($tmpfile);
    return;

}

##############################
# Function: make_positive()
# Descr: to make the regular expression positive and ready for matching by //
# Input: $regex          string containing regexp
# Return: string
# Output: nothing
# Remarks: cuts out the negation from regexp
sub make_positive($) {
    my $regex = $_[0];

    $regex =~ s/^!$/DATE_OFF/o;
    $regex =~ s/^![\s]*(.*)[\s]*$/$1/oi;
    return ($regex);
}

# Function find_output()
sub find_output(@) {
    my ( $entry, $logfile, $lines ) = @_;
# $entry  - reference to the entry in object hash
# $logfile  - name of the file to look at.
# we assume that the file exists if we got there.
# returns the reference to array containing strings with relevant output for this object
# returns undef if output not found or no pattern

    my $class;
    my $key1;
    my $key2;

    my %patterns = (
        # to find the relevant output. format:
        # type of log   => start pattern, end pattern
        'acklog'    => [ '^---[\s]*$', '^(---[\s]*)|([~]+[\s]*)$' ],
        'forwlog'   => [ '^---[\s]*$', '^(---[\s]*)' ],
        'notiflog'  => [ '^---[\s]*$', '^(---[\s]*)' ],
        'updlog'    => [ '<<<',        '(^[\s]*$)|(^>>>)' ],
        'stderrlog' => [ '.*',         '.*' ],
        'stdoutlog' => [ '.*',         '.*' ]
    );
    my %ident = (
        # how to match type/key1/[key2] pair in the block
        # type of log => "string"
        'acklog' => [
            '^\w+ \w+: \[$class\] $key1[\w\s]*$',
            '^\w+ \w+: \[$class\] $key1$key2[\s]*$'
        ],
        'forwlog'  => [ '^$class:[\s]+$key1[\s]*$', '^\w+:[\s]+$key2[\s]*$' ],
        'notiflog' => ['.*'],
        'updlog'   => ['.*'],
        'stderrlog' => ['.*'],
        'stdoutlog' => ['.*']
    );

    # open the file - we assume that the file exists
    open( LOGFILE, "< $logfile" )
        or error( 'E_FOPEN', $logfile, $! );

    # determine type and key(s) of this object
    my $obj = $entry->{"object"};
    my $run = $obj;
    $run =~ s/^([a-z0-9-]+)[\s]+(.*?)[\s]*([,](.*?))?[\s]*$//io;
    ( $class, $key1, $key2 ) = ( $1, $2, $3 );
    $key2 =~ s/,[\s]*// if ($key2);
    if ( !$class || !$key1 ) {
        report( 'E_UNDEF', $obj );
        next;
    }
    my $pat = $logfile;
    $pat =~ s/([a-z]*)[.][0-9]+$//oi;
    $pat = $1;
    if ( !exists $patterns{$pat} || !exists $ident{$pat} ) {
        undef $lines;
        return (undef);
    }
    # expand
    foreach ( @{ $ident{$pat} } ) {
        s/\$class/$class/g;
        s/\$key1/$key1/g;
        s/\$key2/$key2/g if ( defined $key2 );
    }

    undef @{$lines};
    my $curpos;
    my $oldpos;

    $oldpos = tell(LOGFILE);
    my $begin  = 0;
    my $string = <LOGFILE>;
    while ( defined $string ) {
        $curpos = tell(LOGFILE);
        my $path = getvar('CURRENT_DIR');
        #print STDERR "DEBUG: logfile is [$logfile]\n";
        #print STDERR "DEBUG: path is [$path]\n";
        #print STDERR "DEBUG: string is [$string]\n";

        if ( $string =~ /$path/i ) {
            # skip the string with 'test stamp'
            $oldpos = $curpos;
            $string = <LOGFILE>;
            next;
        }

        if ( $string !~ /@{$patterns{$pat}}[0]/ ) {
            # skip
        }
        else {
            #         print "LINE IN BLOCK: $string";
            push @{$lines}, $string;
            $string = <LOGFILE>;
            while ( defined $string
                && !( $string =~ /@{$patterns{$pat}}[1]/ ) )
            {
                $curpos = tell(LOGFILE);
                push @{$lines}, $string;
                #           print "LINE IN BLOCK: $string";
                $oldpos = $curpos;
                $string = <LOGFILE>;
            }
            # we exited because of EOF or match
            # now, check the keys
            # perform "rollback 1 string"
            seek( LOGFILE, $oldpos, 0 );
            my $found_key = 0;
            foreach ( @{$lines} ) {
                if ( $key2 && @{ $ident{$pat} }[1] ) {
                    if ( $_ =~ /@{$ident{$pat}}[1]/i ) {
                        $found_key = 2;
                    }
                }
                elsif ( $_ =~ /@{$ident{$pat}}[0]/i ) {
                    $found_key = 1;
                }
            }
            #         print "KEY FOUND FOR THIS FRAGMENT $class $key1\n";
            return $lines if ( $found_key > 0 );
            undef( @{$lines} );    # otherwise
        }

        $oldpos = $curpos;
        $string = <LOGFILE>;

    }

    close(LOGFILE)
        or error( 'E_FCLOSE', $logfile, $! );

    # no relevant block found in this file, umpfff....

    undef($lines);
    return (undef);
}

# this function matches the regexps against array and updates $entry to contain diagnostics if something failed. doesn't return anything.

sub match_lines(@) {
    my ( $entry, $type, $regexps, $lines ) = @_;

    my $matches    = {};
    my $line_count = 0;

    #   print "<<< TYPE $type ENTRY $entry->{\"object\"} >>>\n";
    #   foreach (@{$lines}) {
    #     print "MATCHING OUTPUT: <<$_>>";
    #   }

    foreach my $string ( @{$lines} ) {
        $line_count++;
        foreach my $regex ( @{$regexps} ) {
            #     print "matching reg ", $regex,"\n";
            eval { $regex =~ /$regex/; };
            if ($@) {
                report($@);
                next;
            }
            unless ( exists $matches->{$regex} ) {
                # match against current string if not used before
                if ( is_negative($regex) ) {    # negative
                    my $temp = make_positive($regex);
                    if (   ( $entry->{'lines'} eq 'multiple' )
                        && ( $string =~ /$temp/ ) )
                    {
                        $matches->{$regex} = $line_count;    #save line count
                    }
                    if (   ( $entry->{'lines'} eq 'single' )
                        && ( $string =~ /$temp/ms ) )
                    {
                        $matches->{$regex} = $line_count;    #save line count
                    }
                }
                elsif (( $entry->{'lines'} eq 'multiple' )
                    && ( $string =~ /$regex/ ) )
                {                                            # positive
                    $matches->{$regex} = $line_count;        # save line count
                }
                elsif (( $entry->{'lines'} eq 'single' )
                    && ( $string =~ /$regex/ms ) )
                {                                            # positive
                    $matches->{$regex} = $line_count;        # save line count
                }
            }
        }
    }

    # now %matches contains matches and line numbers.
    # determine which are real matches, and which are bogus.
    foreach ( @{$regexps} ) {
        if ( is_negative($_) ) {                             # negative
            if ( exists $matches->{$_} ) {                   # but matched
                                                             # bogus match
                push @{ $entry->{ $type . "diag" } },
                    "'$_' at line $matches->{$_}";
            }
        }
        elsif ( !defined $matches->{$_} ) {   #positive but not in the matches
            push @{ $entry->{ $type . "diag" } }, $_;
        }
    }

}
# this function checks if there are any conflicting regexps for the same type of file.
# for example:
# notif: !
# notif: ^bla
# together they are meaningless
# we need to generate error in this case and go to the next entry.
sub check_conflicts($) {
    my $regexps = $_[0];

    my $empty_found  = 0;
    my $normal_found = 0;

    foreach ( @{$regexps} ) {
        $empty_found  = 1 if ( is_empty($_) );
        $normal_found = 1 if ( !is_empty($_) );
    }

    if ( $empty_found && $normal_found ) {
        #unacceptable
        return (undef);
    }
    else {
        # no conflicts
        return (1);
    }

}

##############################
# Function: match_filter()
# Descr: takes the objects, matches their filters with file
# Input:
#         $objects     reference to the hash of hashes.
#                      every hash represents the object
#                      from filters_local.config file in form attr=>value
#         $filters     reference to the hash of hashes.
#                      every hash represents the filter
#                      from filters_local.config in fomr attr=>value
#         $type        type of the logfile (used to determine the type)
#                      is constructed from the sequence <type>LOG
#         $logfile     the name of the logfile
#
# Return: nothing (objects array is updated), dies if I/O error
# Output: nothing
# Remarks: adds "diag: <failed_regexp> <line number if known>" to object if any errors found
sub match_filter(@) {
    my ( $objects, $filters, $type, $logfile ) = @_;
    # $objects  - reference to the array of objects
    # $filters  - reference to a hash of filters
    # $type     - type of the logfile: ack, notif, forw
    # $logfile  - name of the file to look on.

    my $entry;    # to hold current object
    my $lines     = [];                  # to hold relevant output in log file
    my $logstring = getvar('LOGSTRING');

    foreach $entry ( @{$objects} ) {

        # array of filter names
        my $filter_names = $entry->{"body"};
        # array of regexps to match for this object
        my $regexps = [];
        my $lines   = [];

        # all filters mentioned
        foreach my $name ( @{$filter_names} ) {

            if ( $name =~ /^([a-z0-9-_]+)[\s]*\([\s]*(.+)[\s]*\)[\s]*$/i ) {
                my $name_without_args = $1;
                my $tmp               = $2;
                my @args;
                {
                    local $/;
                    undef $/;
                    @args = split( /[\s]*(?<!\\),[\s]*/o, $tmp );
                }

                foreach
                    my $arg_type ( keys %{ $filters->{$name_without_args} } )
                {
                    next if ( $arg_type =~ /filter/ );

                    my $tmp = $arg_type;
                    $tmp =~ s/\$([0-9]{1})/$args[$1]/g;

                    if (   ( $arg_type eq $type )
                        || ( ( $tmp eq $type ) && ( $arg_type =~ /\$/ ) ) )
                    {    #our type to analyse
                        foreach my $regexp (
                            @{ $filters->{$name_without_args}->{$arg_type} } )
                        {
                            my $reg = $regexp;
                            $reg
                                =~ s/\$([0-9]{1})/defined $args[$1]? $args[$1]:""/eg;
                            $reg =~ s/\\,/,/go;
                            push @{$regexps}, $reg;
                        }
                    }
                    elsif ( $logstring !~ /$tmp/i ) {
                        error("ERROR: Invalid log type specified: $tmp\n");
                    }
                }    # end of 'foreach' through "ack, notif, forw, ..."
            }    # if name contains arguments
            else {    # name with no arguments
                push @{$regexps}, @{ $filters->{$name}->{$type} }
                    if ( defined @{ $filters->{$name}->{$type} } );
            }

        }

        # move further if no regexps defined for this type of log
        next if ( !@{$regexps} );

        # check possible conflicts within regexps of the same file type
        if ( !check_conflicts($regexps) ) {
            # all regexp become failed
            foreach ( @{$regexps} ) {
                push @{ $entry->{ $type . "diag" } }, "$_ \t<<< CONFLICT";
            }
            # go to the next entry
            next;
        }

        # now we work with regexps of the same type (empty or non-empty)
        unless ( -e $logfile ) {
# all regexps for this type FAILED if file doesn't exist, except from EMPTY (!)
            foreach ( @{$regexps} ) {
                push @{ $entry->{ $type . "diag" } }, "$_"
                    if ( !is_empty($_) );
            }
            # go to the next entry
            next;
        }

        # find object type and relevant output
        if ( $entry->{'object'} eq 'all' ) {
            # read the file into $lines
            open( LOGFILE, "< $logfile" )
                or error( 'E_FOPEN', $logfile, $! );
            $lines = [];
            my $path = getvar('CURRENT_DIR');
            foreach my $string (<LOGFILE>) {
                push @{$lines}, $string
                    unless ( $string =~ /TEST RUN $path/i );
            }
        }
        else {
            $lines = [] if ( !find_output( $entry, $logfile, $lines ) );
        }

        # now $lines holds output to be checked or empty array

        # check which regexps are unmatchable
        if ( scalar( @{$lines} ) == 0 ) {
            foreach ( @{$regexps} ) {
                push @{ $entry->{ $type . "diag" } }, "$_"
                    if ( !is_empty($_) );
            }
            next;
        }
        else {
            my $flag = 0;
            foreach ( @{$regexps} ) {
                if ( is_empty($_) ) {
                    push @{ $entry->{ $type . "diag" } }, $_;
                    $flag = 1;
                }
            }
            next if ( $flag == 1 );
        }

        # otherwise do normal pattern-matching
        if ( $entry->{'lines'} eq 'single' ) {
            # make it an array with one string
            my $tmp = join( '', @{$lines} );
            undef @{$lines};
            push @{$lines}, $tmp;
        }
        match_lines( $entry, $type, $regexps, $lines );

    }
    return (1);
}

##############################
# Function: gather_objects()
# Descr:  puts together the hash of parsed objects to prepare for matching
# handy for matching the whois output
# Input: $FILE    pointer to the file
#        $list    reference to the hash where object should be stored
#                 in form: name => object_as_string
#                 name is a string: "class_attr:primary_key"
#                 in some cases key is more complex (person, role, route):
#                 "class_attr:prefix,AS" or "class_attr:name,nic-hdl".
#                 all this hassle is made to make sure they are unique!
# Return: 1
# Output: nothing
# Remarks:

sub gather_objects($$) {
    my $FILE = $_[0];
    my $list = $_[1];

    while (<$FILE>) {
        #	  print STDERR "string [$_]\n";
        my $string = $_;
        if ( $string =~ /^[a-z-]+/oi ) {
            my $object = "";
            #while ($string && ($string =~ /^[a-z0-9-]+/oi) ) {
            while ( $string && ( $string !~ /^$/oi ) ) {
                #				print STDERR "string [$string]\n";
                #enter object parsing mode
                #$string =~ /^(.*?):[\s]*(.*?)[\s]*$/i;
                #$object = $object.$1.":".$2."\n";
                $object = $object . $string;
                $string = <$FILE>;
            }
            #print STDERR "OBJECT [$object]\n";
            # object in $object;
            my $tmp = $object;
            my $name;
            if ( $object =~ /^(route):(.*?)[\n](.*?)(origin):(.*?)[\n]/iosm )
            {
                $name = $1 . " " . $2 . ", " . $5;
            }
            elsif (
                $object =~ /^(person):(.*?)[\n](.*?)(nic-hdl):(.*?)[\n]/iosm )
            {
                $name = $1 . " " . $5;
            }
            elsif (
                $object =~ /^(role):(.*?)[\n](.*?)(nic-hdl):(.*?)[\n]/iosm )
            {
                $name = $1 . " " . $5;
            }
            elsif ( $object =~ /^(.+?):(.+?)[\n]/iom ) {
                $name = $1 . " " . $2;
            }
            $name =~ s/\s+/ /g;
            #print STDERR "NAME [$name]\n";
            #print STDERR "OBJECT [$tmp]\n";
            $list->{$name} = $tmp;
        }
    }
    return (1);
}

sub gather_objects_whois($$) {
    my $objs = shift;
    my $list = shift;

    foreach my $object ( @{$objs} ) {
        my $tmp = $object;
        my $name;
        if ( $object
            =~ /^(route):[\s]*(.*?)[\n](.*?)(origin):[\s]*(.*?)[\n]/iosm )
        {
            $name = $1 . " " . $2 . ", " . $5;
        }
        elsif ( $object
            =~ /^(person):[\s]*(.*?)[\n](.*?)(nic-hdl):[\s]*(.*?)[\n]/iosm )
        {
            $name = $1 . " " . $5;
        }
        elsif ( $object
            =~ /^(role):[\s]*(.*?)[\n](.*?)(nic-hdl):[\s]*(.*?)[\n]/iosm )
        {
            $name = $1 . " " . $5;
        }
        elsif ( $object =~ /^(.+?):[\s]*(.+?)[\n]/iom ) {
            $name = $1 . " " . $2;
        }
        #print STDERR "\nname  [$name]\n\n";
        #print STDERR "\nobject  [$tmp]\n\n";
        $list->{$name} = $tmp;
    }
    return (1);
}

##############################
# Function: match_whois()
# Descr: take whois query and match output with whois_output file
# Input:
#       $objects   reference to the hash containing object hashes,
#                  in the form attr=>value
#       $filters   reference to the hash containing filters hashes,
#                  in the form attr=>value
#
# Return: nothing or die if I/O error or syntax error
# Output: nothing
# Remarks: modifies the objects to put diagnostics in, if error occured.
sub match_whois($$) {
    my $objects    = $_[0];
    my $filters    = $_[1];
    my $objectfile = getvar('CURRENT_DIR') . "/" . getvar('WHOIS_FILE');
    my $expected   = {};
    my $found      = {};

    # is whois file required? get all objects as strings if yes.
    # key of the hash is entry->{"object"}
    foreach my $entry ( @{$objects} ) {
        my $queries = $entry->{"whois"};
        foreach my $query ( @{$queries} ) {
            if ( !is_negative($query) ) {    # at least one positive query
                open( WHOIS_FILE, "< $objectfile" )
                    or error( 'E_FOPEN', $objectfile, $! );

                gather_objects( *WHOIS_FILE, $expected );

                close(WHOIS_FILE)
                    or error( 'E_FCLOSE', $objectfile, $! );
                last;
            }
        }
    }

    foreach my $entry ( @{$objects} ) {
        my $queries = $entry->{"whois"};
        foreach my $tmp ( @{$queries} ) {

            # what type of query is it?
            my $query = $tmp;
            if ( is_negative($query) ) {
                $query = make_positive($query);
            }
            my $obj = $entry->{"object"};

            # gather queries and see if every positive result is defined
            if ( ( !is_negative($tmp) ) && ( !exists $expected->{$obj} ) ) {
                error( 'E_NOENTRY', $tmp, $obj );
            }

            # compose the query
            # take the object's first key as the search key
            my $run = $obj;
            $run =~ s/^(.*?)[\s]+(.*?),[\s]*(.*?)$/$1 $2/i;
            $run = " -G -B -r -x -T " . $run;

            my $whois;
            if ( exists $CONFIG->{'QUERY_AF'}
                && getvar('QUERY_AF') =~ '.*INET6.*' )
            {
                $whois = IO::Socket::INET6->new( Domain => AF_INET6,
                    Proto => 'tcp', PeerHost => getvar('WHOIS6_HOST'),
                    PeerPort => getvar('SVWHOIS_PORT') )
                    or error( 'E_WHOIS', 'whois', $! );
            }
            else {    #default is ipv4
                $whois = IO::Socket::INET->new(
                    getvar('WHOIS_HOST') . ":" . getvar('SVWHOIS_PORT') )
                    or error( 'E_WHOIS', 'whois', $! );
            }
            $whois->print( $run, "\n" );

            #print STDERR "DEBUG: query [$run]\n";

            my @received_lines;

            my $line;
            do {
                $line = <$whois>;
                push @received_lines, $line if ($line);
            } while ($line);

            my @objs;
            my $object;
            foreach my $line (@received_lines) {
                if ( $line =~ /^$/o ) {
                    if ( $object && ( $object !~ /^$/o ) ) {
                        chomp $object;
                        push @objs, $object;
                    }
                    $object = "";
                }
                elsif ( $line !~ /^%/o ) {
                    $object .= $line;
                }
            }

            gather_objects_whois( \@objs, $found ) if (@objs);

            #close (WHOIS)
            #  or error ('E_FCLOSE', "whois: $run", $!);

            # change the dates
            if ( $query =~ /DATE_ON/io ) {
                # put today's date in expected.
                my $date = strftime( "%Y%m%d", localtime );
                if ( exists $expected->{$obj} ) {
                    # add the date if it is not there
                    unless (
                        $expected->{$obj} =~ /changed:(.*?)[\s]+[\d]+[\s]*/ )
                    {
                        $expected->{$obj}
                            =~ s/changed:      (.*?)[\n]/changed:$1 $date\n/iosm;
                    }
                }
            }
            elsif ( $query =~ /DATE_OFF/io ) {
                # filter dates out in found object & expected object
                if ( exists $found->{$obj} ) {
                    $found->{$obj}
                        =~ s/changed:(.*?)[\s]+([0-9]*)[\s]*[\n]/changed:$1\n/iosm;
                    $expected->{$obj}
                        =~ s/changed:(.*?)[\s]+([0-9]*)[\s]*[\n]/changed:$1\n/iosm;
                }
            }

            if ( $query !~ /EXACT/io ) {
        # if no exact matching, cut out the spaces between attribute and value
                if ( exists $expected->{$obj} ) {
                    $expected->{$obj} =~ s/^(\S+):\s+/$1:/mgi;
                }
                if ( exists $found->{$obj} ) {
                    $found->{$obj} =~ s/^(\S+):\s+/$1:/mgi;
                }
            }

            # match expected objects with found ones
            #if ( exists $found->{$obj} ) {
            #  $found->{$obj} =~ s/:\s*/:/g;
            #}

            my $expected_tmp;
            if ( exists $expected->{$obj} ) {
                $expected_tmp = $expected->{$obj};
                $expected_tmp =~ s/\n$//s;
            }
            #$found->{$obj} =~ s/\n/]\n/gm;
            #$expected_tmp =~ s/\n/]\n/gm;
            #print STDERR "\nfound\n\n[$found->{$obj}] with name [$obj]\n\n";
            #print STDERR "\nexpected\n\n[$expected_tmp]\n\n";

            if (   ( !is_negative($tmp) )
                && ( exists( $found->{$obj} ) )
                && $expected_tmp
                && ( $expected_tmp eq $found->{$obj} ) )
            {
                #OK;
            }
            elsif ( is_negative($tmp) && !exists $found->{$obj} ) {
                #OK;
            }
            else {
                # FAILURE
                push @{ $entry->{"whoisdiag"} }, $tmp;
                if ( $expected_tmp && exists $found->{$obj} ) {
                    my @lines_found    = split( "\n", $found->{$obj} );
                    my @lines_expected = split( "\n", $expected_tmp );
                    my $c              = 0;

                    foreach my $line (@lines_expected) {
                        if ( $line ne $lines_found[$c] ) {
                            push @{ $entry->{"whoisdiag"} }, "In line $c:\n";
                            push @{ $entry->{"whoisdiag"} },
                                "expected [$line]\nfound    [$lines_found[$c]]\n";
                        }
                        $c++;
                    }
                }
                if ( !$expected_tmp && !is_negative($tmp) ) {
                    push @{ $entry->{"whoisdiag"} },
                        "no defined object for [$obj]\n";
                }
                if ( !exists $found->{$obj} && !is_negative($tmp) ) {
                    push @{ $entry->{"whoisdiag"} },
                        "no whois object for [$obj]\n";
                }
            }
        }
    }
    return (1);
}

##############################
# Function: match_query()
# Descr: take whois query and match output with query file
#				note: this can be done only once!
# Input:
#       $objects   reference to the hash containing object hashes,
#                  in the form attr=>value
#       $filters   reference to the hash containing filters hashes,
#                  in the form attr=>value
#
# Return: nothing or die if I/O error or syntax error
# Output: nothing
# Remarks: modifies the objects to put diagnostics in, if error occured.
sub match_query($$) {
    my $objects = $_[0];
    my $filters = $_[1];
    my $query;
    my @expected_lines;
    my @received_lines;
    my $objectfile = getvar('CURRENT_DIR') . "/" . getvar('QUERY_FILE');
    my $object;

   # in fact, we will match only one query from object 'all' against one file.
    foreach my $entry ( @{$objects} ) {
        if ( $entry->{"object"} =~ /all/io ) {
            my $queries = $entry->{"query"};
            foreach my $t ( @{$queries} ) {
                $query  = $t;
                $object = $entry;
                last;
            }
        }
    }

    if ( !$query ) {
        return (1);
    }

    if ( $query !~ /^!/io ) {
        # require query file
        open( QUERY_FILE, "< $objectfile" )
            or error( 'E_FOPEN', $objectfile, $! );
        @expected_lines = <QUERY_FILE>;
        close(QUERY_FILE)
            or error( 'E_FCLOSE', $objectfile, $! );

    }

    my $query_orig = $query;
    $query =~ s/^[\s]*EXACT[\s]+//i;

    my $whois;

    if ( exists $CONFIG->{'QUERY_AF'} && getvar('QUERY_AF') =~ /.*INET6.*/ ) {
        $whois = IO::Socket::INET6->new( Domain => AF_INET6, Proto => 'tcp',
            PeerHost => getvar('WHOIS6_HOST'),
            PeerPort => getvar('SVWHOIS_PORT') )
            or error( 'E_WHOIS', 'whois', $! );
    }
    else {    #default is ipv4
        $whois = IO::Socket::INET->new(
            getvar('WHOIS_HOST') . ":" . getvar('SVWHOIS_PORT') )
            or error( 'E_WHOIS', 'whois', $! );
    }
    $whois->print( $query, "\n" );

    #print STDERR "DEBUG: query [$query]\n";

    my $line;
    do {
        $line = <$whois>;
        push @received_lines, $line if ($line);
    } while ($line);

    if ( $query_orig !~ /EXACT/io ) {
        # if no exact matching, cut out the spaces between attribute and value
        # cut out the end-of-line comment from source attribute if any
        foreach (@received_lines) {
            s/^(\S+):\s+/$1:/gmi;
            s/^(source):(\S+)\s*#.*$/$1:$2/gmi;
        }
        foreach (@expected_lines) {
            s/^(\S+):\s+/$1:/gmi;
            s/^(source):(\S+)\s*#.*$/$1:$2/gmi;
        }
    }

    for ( my $i = 0; $i < scalar(@expected_lines); $i++ ) {
        if (   $expected_lines[$i]
            && $received_lines[$i]
            && $expected_lines[$i] eq $received_lines[$i] )
        {
            if ( $query =~ /^!/io ) {
                push @{ $object->{"querydiag"} },
                    "In line $i: expected [$expected_lines[$i]] received [$received_lines[$i]]\n";
            }
        }
        else {
            if ( $query !~ /^!/io ) {
                push @{ $object->{"querydiag"} }, "In line $i: expected [ ",
                    $expected_lines[$i] ? $expected_lines[$i] : "",
                    "] received [ ",
                    $received_lines[$i] ? $received_lines[$i] : "", "]\n";
            }
        }
    }

    $whois->close();

    return (1);
}

##############################
# Function: parse_rip_config()
# Descr: take necessary values from rip.config
# Input: $configfile         config file name
# Return: $CONFIG (reference to hash), updated with new values
# Output: fatal errors
# Remarks: (hopefully) improve configurability!!!
sub parse_rip_config() {
    my $configfile = getvar('RIP_CONFIG');
    my @vars       = (
        qw/ SVWHOIS_PORT SVCONFIG_PORT SVMIRROR_PORT GPGCMD UPDSOURCE ACKLOG UPDLOG FORWLOG NOTIFLOG TMPDIR RIPADMIN/
    );

    # format is multiline vars, like
    # VARNAME whitespaces VALUE

    open( FILE, "< $configfile" )
        or die "ERROR: cannot open file $configfile : $!";
    while (<FILE>) {
        foreach my $name (@vars) {
            if (/^($name)[\s]*?(.*?)[\s]*?$/i) {
                if ( !getvar($1) ) {
                    setvar( $1, $2 );
                }
                else {
                    # if multiline, split by "\n"
                    my $tmp = getvar($1);
                    $tmp .= "\n$2";
                    setvar( $1, $tmp );
                }
            }
        }
    }

    # resolve unresolved names
    for my $key ( sort keys %{$CONFIG} ) {
        $CONFIG->{$key}
            =~ s/\$(\w+)/defined($CONFIG->{$1})?$CONFIG->{$1}:"\$$1"/eg;
        $CONFIG->{$key} =~ s/^[\s]*//o;
        $ENV{$key} = $CONFIG->{$key};
    }

    close(FILE)
        or die "ERROR: cannot close file $configfile :$!";
    return ($CONFIG);
}

##############################
# Function: parse_config()
# Descr: take paths from config to a hash
# Input: $configfile            config file name
# Return: reference to hash with configuration data in form varname=>value
# Output: fatal errors
# Remarks: variables containing others ($RIP_CONFIG) are substituted with actual values. Somethings gets added to this hash when parsing the command line.
sub parse_config() {
    my $configfile;
    $configfile = getvar('CONFIG');

    # required config variables
    my @REQUIRED
        = qw/ BASEDIR CONFDIR BINDIR DATADIR LOGDIR DUMPDIR WHOISDIR TEST_TMPDIR
        FILTERS
        DBUPDATE DBUPDATE_GEN
        DBUPDATE_TEXT DBUPDATE_MAIL DBUPDATE_NET DBUPDATE_TRACE
        WHOIS_HOST WHOIS6_HOST
        MAKE_DB MAKE_DB_FLAGS LOAD_FILE
        RESET_DB
        RIP_CONFIG
        REPORTLOG SUMMARYLOG STDOUTLOG STDERRLOG
        LOADER_FILE DBUPDATE_FILE TEST WHOIS_FILE DEBUG QUERY_FILE
        FILTERS_LOCAL EXCLUDE
        /;

    open( FILE, "< $configfile" )
        or die "ERROR: cannot open file $configfile : $!";
    while (<FILE>) {
        if (/^([0-9a-z_]+)[\s]*?[=][\s]*(.*?)[\s]*$/io) {
            setvar( $1, $2 );
        }
    }

    # resolve names
    for my $key ( sort keys %{$CONFIG} ) {
        $CONFIG->{$key}
            =~ s/\$(\w+)/defined($CONFIG->{$1})?$CONFIG->{$1}:"\$$1"/eg;
    }

    if ( %{$CONFIG} ) {
        foreach ( sort @REQUIRED ) {
            if ( !getvar($_) ) {
                die "$ERR->{'E_CONFVAR'} $_\n";
            }
        }
    }

    parse_rip_config();

    # for log types
    my $logstring = "";

    # check vars
    if ( %{$CONFIG} ) {
        #foreach (sort @REQUIRED)	{
        # if (! getvar($_))	{
        #    die "$ERR->{'E_CONFVAR'} $_\n";
        # }
        #}
        for ( keys %{$CONFIG} ) {
# add types of logs to $CONFIG as a string which will be used by other functions:
            if (/^([A-Z]+)LOG$/io) {
                $logstring = $logstring . lc($1) . " ";
            }
        }
        setvar( 'LOGSTRING', $logstring );
    }

    # error counter
    $CONFIG->{'ERRCOUNT'} = 0;

    trace();

    return ($CONFIG);

}

# Function: exec_dbupdate()
# Descr: submit objects to database through dbupdate
# Input: $commandline   dbupdate command line
#                       '-t' for tracing is also added in this function if
#                       '-d' command line option was set (trace_dbupdate)
#
# Return: die if failed, number of objects if ok
# Output: error report and underlying functions' messages
sub exec_dbupdate($) {
    my $commandline = $_[0];
    my $objects     = 0;

    $SIG{'CHLD'} = 'DEFAULT';
    my $input_redirect;
    my $hostname = `hostname`;
    chomp($hostname);

    if ( $commandline =~ /<[\s]*(.*)[\s]*$/ ) {
        $input_redirect = $1;
        $commandline
            =~ s/<[\s]*(.*)[\s]*$//;    # delete redirector and the filename
    }

    # split the command line into space seperated arguments
    # but keep anything between quotes ("") as one argument
    my @args = ();
    while ( $commandline =~ /\"/ ) {
        $commandline =~ /(.*?)"(.*?)"\s*(.*)$/s;
        my ( $bit1, $quoted, $bit3 ) = ( $1, $2, $3 );
        $commandline = $bit3;
        push @args, split( '[\s]+', $bit1 );
        push @args, $quoted;
    }
    push @args, split( '[\s]+', $commandline );

    my $command = splice( @args, 0, 1 );
    my $pid = open3( *HIS_IN, *HIS_OUT, *HIS_ERR, $command, @args );
    # not checking the open3 because it is useless, we're using shell :(
    # this needed to be reimplemented by using fork-exec mechanism! :-E
    # we just check if dbupdate exists and is executable

    if ($input_redirect) {
        open( INPUT, "< $input_redirect" )
            or error( 'E_FOPEN', $input_redirect );
        while (<INPUT>) {
            print HIS_IN $_;
        }
        close(INPUT)
            or error( 'E_FCLOSE', $input_redirect );
    }
    close(HIS_IN)
        or error( 'E_COMMRUN', $commandline, $! );

    my $selector = IO::Select->new();
    $selector->add( *HIS_OUT, *HIS_ERR );

    my $errfile = getvar('STDERRLOG');
    open( STDE, "> $errfile" )
        or error( 'E_FOPEN', $errfile )
        if ( getvar('STDERR') );
    my $outfile = getvar('STDOUTLOG');
    open( STDO, "> $outfile" )
        or error( 'E_FOPEN', $outfile )
        if ( getvar('STDOUT') );

    while ( my @ready = $selector->can_read ) {
        foreach my $fh (@ready) {
            if ( fileno($fh) == fileno(HIS_ERR) ) {
                my $string = scalar <HIS_ERR>;
                if ( getvar('STDERR') && $string ) {
                    print STDE $string;
                    error( 'E_DIE', $commandline ) if ( $string =~ /died/o );
                }
            }
            else {
                my $string = scalar <HIS_OUT>;
                if ($string) {
                    report($string)
                        if ( getvar('TRACE_DBUPDATE') ); # dbupdate tracing on
                    error( 'E_REFUSE', $commandline )
                        if ( $string =~ /Connection refused/o );
                    $objects = $1
                        if ( $string
                        =~ /^Number of objects processed successfully:[\s]+([0-9]+)[\s]*$/o
                        );
                    print STDO $string if ( getvar('STDOUT') );
                }
            }
            $selector->remove($fh) if eof($fh);
        }
    }

    my $dbupdate_state = getvar('TMPDIR')
        . "/dbupdate.state."
        . $hostname . "."
        . $pid . ".tmp";
    check_dbupdate($dbupdate_state);

    my $kid;
    do {
        $kid = waitpid( $pid, 0 );
    } until ( $kid > 0 );
    my $signal = ( $? & 127 );
    if ( $signal == $signo{'INT'} ) { $received_interrupt = 1; }
    my $return_code  = ( $? >> 8 );
    my $defined_code = getvar('DBUPDATE_IGNORE_EXIT_CODE');
    # if nothing was set, set expected exit code to 0
    unless ( defined $defined_code ) {
        setvar( 'DBUPDATE_IGNORE_EXIT_CODE', 0 );
        $defined_code = 0;
    }
    if ( ($return_code) != $defined_code ) {
        error( 'E_COMMRUN', $commandline,
            "expected exit code != received : $defined_code != $return_code"
        );
    }
    delvar('DBUPDATE_IGNORE_EXIT_CODE');

    close(HIS_OUT)
        or error( 'E_COMMRUN', $commandline, $! );
    close(HIS_ERR)
        or error( 'E_COMMRUN', $commandline, $! );
    close(STDE);
    close(STDO);

    return ($objects);

}

##############################
# Function: run_update()
# Descr: submit objects to database through dbupdate || networkupdate
#        actually, it is a wrapper to exec_dbupdate
# Input:
#        $updatefile        file containing the update itself
#                           as well as customized dbupdate flags
#                           see dataset.config (DBUPDATE_FLAGS variable)
#
#        $force             flag to indicate that the number of
#                           submitted objects should be explicitly the same
#                           as the number of objects which were OK
#                           this is to make sure that initial
#                           submission is ok
# Return: die if failed, 1 if ok
# Output: error report and underlying functions' messages
# Remarks: checks the date to sleep (5) if we are at midnight, to avoid a mess with log filenames, so switch the dates safely.
sub run_update(@) {
    my $updatefile = shift(@_);
    my $tempfile   = $updatefile;
    $tempfile =~ s/^(.*)\/(.*)$/$2/;
    $tempfile = getvar('TEST_TMPDIR') . "/$tempfile.$$";

    my $force
        = shift(@_);    # flag to indicate that it should be always successful
    my $update = [];
    my $flags;

    error( 'E_COMMRUN', getvar('DBUPDATE'), "file open error" )
        unless ( ( -e getvar('DBUPDATE') ) && ( -x getvar('DBUPDATE') ) );

    # take dbupdate flags, comments and actual update from
    filter( $updatefile, $tempfile, $update );

    # take out flags for dbupdate
    #$flags = pop (@{$update});
    $flags = getvar('DBUPDATE_FLAGS') || getvar('DBUPDATE_FLAGS_EXT');

    # add comments to the report
    foreach ( @{$update} ) {
        report($_);
    }

    # if flags are not defined, take defaults
    if ( !$flags || ( $flags =~ /^[\s]*$/ ) ) {
        $flags = '$DBUPDATE_GEN $DBUPDATE_TEXT ';
    }
    $flags =~ s/^\$[\s]+//;    # cut out leading '$ '

    # expand configured flags to their real values
    $flags =~ s/\$(\w+)/defined($CONFIG->{$1})?$CONFIG->{$1}:''/mseg;
    # in case if one of reserved filenames is mentioned
    $flags =~ s/$updatefile/$tempfile/ms;
    chomp($flags);

    # in temporary file there are RPSL objects OR obj + mail headers;
    # submit the temporary file and delete it

    my $commandline = "";

    if ( getvar('DBUPDATE_FLAGS_EXT') ) {
        $flags = "$flags $tempfile";
    }

    if ( !getvar('DBUPDATE_FLAGS_EXT') && !getvar('DBUPDATE_FLAGS') ) {
        $flags = "$flags $tempfile";
    }

    $commandline = $flags;
    $commandline = getvar('DBUPDATE_TRACE') . " $commandline"
        if ( getvar('TRACE_DBUPDATE') );    #add tracing if option is set
    $commandline = getvar('DBUPDATE') . "    $commandline";

    #report ("[COMMAND: $commandline]\n");

    my $object_expected;
    if ( $force == 1 ) {
        $object_expected = count_objects($tempfile);
    }
    # run dbupdate, to read the output with pipe.
    # return number of objects which were OK. or undef if something failed

    my $obj = exec_dbupdate($commandline);

    if ( defined $obj ) {
        if ( defined $object_expected ) {
            if ( $object_expected != $obj ) {
                error( 'E_COMMOBJ', $commandline, $object_expected, $obj );
            }
        }
    }
    else {
        error( 'E_COMMRUN', $commandline, $! );
    }
    return (1);
}

##############################
# Function: remove_tmp()
# Args: no arguments
# Return: nothing
# Descr: delete tmp files from tmp directory.
#
sub remove_tmp() {
    my $tmpdir = getvar('TEST_TMPDIR');

    my @files = glob("$tmpdir/*.$$");
    push @files, glob("$tmpdir/*.$$.gpg");
    foreach (@files) {
        #unlink $_;
    }
}

##############################
# Function: rotate_logs()
# Descr: rotate log files. The log files from 'log' directory are appended to their collegues in 'dump' directory. The reason is that we need to parse the output from each submission (update) separately to be able to isolate the results of each test.
# mode: 0 - rotate dbupdate logs + stderr stdout + debug log
#        1 - rotate report log
#        2 - rotate summary log
#
# Return: nothing
# Output: none
sub rotate_logs($) {
    my $mode = $_[0];
    my $tmp;

    #my $tmp = uc(getvar("LOGSTRING"));

    if ( $mode == 0 ) {
        $tmp = "ACK NOTIF FORW UPD STDERR STDOUT";
    }
    elsif ( $mode == 1 ) {
        $tmp = "REPORT";
    }
    elsif ( $mode == 2 ) {
        $tmp = "SUMMARY";
    }

    my @logs = split( '[\s]+', $tmp );

    foreach my $log_type (@logs) {

        my $logname      = getvar( $log_type . "LOG" );
        my $logname_dump = getvar( "DUMP" . $log_type . "LOG" );

        # empty current log file and push contents to the dump

        # make sure we've started and $logname is defined!
        if ( defined $logname && -e $logname ) {
            open( DUMP, ">> $logname_dump" )
                or die "Cannot open file $logname_dump $!";
            open( LOG, "< $logname" )
                or die "Cannot open file $logname $!";
            while (<LOG>) {
                print DUMP $_;
            }
            close(DUMP)
                or die "Cannot close $logname_dump: $!";
            close(LOG)
                or die "Cannot close $logname: $!";
            # delete the log
            unlink($logname);
        }
    }

    # debug rotating
    if ( $mode == 0 ) {
        my $debug = getvar('DEBUG');
        if ( -e $debug ) {
            my $rotated = getvar('CURRENT_DIR');
            $rotated =~ s/\//_/og;
            $rotated = getvar('DUMPDIR') . "/$rotated.DEBUG";
            rename( $debug, $rotated );
        }
    }

}

##############################
# Function: count_objects()
# Descr: count the number of objs based on "source" line.
# Input: $file       filename to count objects in
# Return: number of objs
# Output: none
sub count_objects($) {
    my $file = $_[0];

    open( FILE, "< $file" )
        or error( 'E_FOPEN', $file, $! );

    my $objects = grep ( /^source:/, <FILE> ) || 0;

    close(FILE)
        or error( 'E_FCLOSE', $file, $! );

    return ($objects);
}

# simple wrapper to filter file and run it through the loader
# dies if something goes wrong
# Function: run_loader()
# Args:      $_[0]            the relative filename for the loader file
# Output:   nothing or error reports
# Return:   1 if everything went ok, reports errors otherwise.

sub run_loader($$) {
    my $source  = $_[0];
    my $file    = $_[1];
    my $tmpfile = $file;
    $tmpfile =~ s/^(.*)\/(.*)$/$2/;
    $tmpfile = getvar('TEST_TMPDIR') . "/$tmpfile.$$";
    my $ret       = [];
    my $temporary = 0;

    unless ( -e $file ) {
        # load empty file if no loader file
        open( TMP, "> $file" )
            or error( 'E_FCLOSE', $file, $! );
        close(TMP)
            or error( 'E_FCLOSE', $file, $! );
        $temporary = 1;
    }

    # $tmpfile contains now objects with our $source
    return unless filter( $file, $tmpfile, $ret, $source );

    # add comments to the report
    foreach ( @{$ret} ) {
        report($_);
    }

    # how many objects we need to load into the DB? - based on "source" line
    my $object_expected = count_objects($tmpfile);

    $SIG{'CHLD'} = 'DEFAULT';
    my $commandline = "/bin/sh "
        . getvar('MAKE_DB')
        . "  -s $source "
        . getvar('MAKE_DB_FLAGS')
        . " $tmpfile 2>&1 |";
    # dirty hack follows - after the first run, replace make_db with reset_db
    setvar( 'MAKE_DB', getvar('RESET_DB') );

    my $pid = open( MAKE_DB, $commandline )
        or error( 'E_COMMRUN', $commandline, $! );

    my @status;
    my $object_real = 0;

    while (<MAKE_DB>) {
        # gather status for error reporting
        push @status, $_;
        # exception if conn.refused
        error( 'E_REFUSE', $commandline ) if (/Connection refused/);
        # get number of objects
        $object_real = $1 if (/[\s]+([0-9]+)[\s]+objects[\s]+OK/o);
    }

    unless ( close(MAKE_DB) ) {
        # report the whole make_db output if something goes wrong
        foreach (@status) {
            s/%/%%/;
            report($_);
        }
        error( 'E_COMMRUN', $commandline, $! );
    }

    if ( $object_expected != $object_real ) {
        # print diagnostics if something wrong
        foreach (@status) {
            report($_);
        }
        error( 'E_COMMOBJ', $commandline, $object_expected, $object_real );
    }

    if ( getvar('TRACE_MAKEDB') ) {
        foreach (@status) {
            report($_);
        }
    }

    # delete pseudo-loader file
    unlink($file) if ( $temporary == 1 );

    return (1);
}

##############################
# Function: make_db()
# Descr: clear keyring; make the database; empty and fill;
#        rebuild radix tree!!!!
#	THEN run dbupdate to submit the objects as initial state.
# Input: nothing
# Return: 1 if successfully, die otherwise
# Output: error report and underlying function messages
sub make_db() {
    my $file;

    # LOADER
    $file = getvar('LOADER_FILE');
    my @sources_list = split( '\n', getvar('UPDSOURCE') );
    foreach my $source (@sources_list) {
        $source =~ /^[\s]*([a-z0-9-]+)[\s]+/i;
        $source = $1;
        run_loader( $source, $file ); # may be run with empty/nonexistent file
    }

    # DBUPDATE
    $file = getvar('DBUPDATE_FILE');
    if ( -e "$file" ) {
        run_update( $file, 1 );       # run only if file exists
    }

    #  rotate_logs(0);
    remove_tmp();

    return (1);

}

##############################
# Function: print_report()
# Descr: is printed for each test
# Input: $objects        reference to objects hash of hashes. each
#                        object is represented by its hash, attr=>value
# Return: the result of the test (OK | FAILED)
# Output: cute reportje
sub print_report($) {
    my $objects = $_[0];

    my @diag = split( '[\s]+', getvar('LOGSTRING') );
    # add whois diagnostics to LOGSTRING
    push @diag, "whois";
    push @diag, "query";
    push @diag, "script";
    my $res = "OK";

    foreach my $entry ( @{$objects} ) {
        my $flag = 0;
        foreach my $diag_type (@diag) {
            if ( exists $entry->{ $diag_type . "diag" } ) {
                $flag = 1;
                # now if something failed, we can exit the loop
                last;
            }
        }
        if ( $flag == 0 ) {
            $entry->{"result"} = "OK";
        }
        else {
            $entry->{"result"} = "FAILED";
            # this is particular test's result
            $res = "FAILED";
        }
    }

    report( "\n%% TEST %s \n\n", $res );

    # print diagnostics here
    if ( $res eq 'FAILED' ) {
        report("%%\n%% The following object(s) FAILED: \n%%\n");
        foreach my $entry ( @{$objects} ) {
            if ( $entry->{"result"} eq "FAILED" ) {
                report( "%% object: %s \n", $entry->{'object'} );
                foreach my $diag_type (@diag) {
                    if ( exists $entry->{ $diag_type . "diag" } ) {
                        foreach my $string (
                            @{ $entry->{ $diag_type . "diag" } } )
                        {
                            my $type = uc($diag_type);
                            report( "%% %s",   $type );
                            report( ": %s \n", $string );
                        }
                    }
                }
                report("%%\n");
            }
        }
    }

    report( "%% Finished: %s", `date` );

    report("<<< End of Test Run >>>\n");

    return ($res);
}

##############################
# Function: trace($)
# Descr: print out $CONFIG values
# Input: nothing
# Return: nothing
# Output: contents of $CONFIG hash
sub trace() {

    if ( getvar('TRACE_TEST') ) {
        print("TEST_TRACE: TEST TRACING START\n");
        for my $key ( sort keys %{$CONFIG} ) {
            print("TEST_TRACE: $key = $CONFIG->{$key}\n");
        }
        print("TEST_TRACE: TEST TRACING END\n");
    }

}

##############################
# Function: set_test_variables($)
# Descr: checks the 'test' file for defined variables and sets them for this test
# Input: nothing
# Return: nothing
# Output: contents of $CONFIG hash
sub set_test_variables() {

    my $testfile = getvar('TEST');
    my %thash;
    open( FILE, "< $testfile" )
        or error( 'E_FOPEN', $testfile, $! );
    while (<FILE>) {
        if (/^\$([a-z0-9_]+)=(.+)$/i) {
            my $var = uc($1);
            my $val = $2;
            $thash{$var}
                ? ( $thash{$var} .= "\n" . $val )
                : ( $thash{$var} = $val );
        }
    }
    foreach my $var ( keys %thash ) {
        setvar( $var, $thash{$var} );
        #    report ("DEBUG: setting [$var] to [$thash{$var}]\n");
    }
    close(FILE);

}

##############################
# Function: filter($)
# Descr: filter out comments followed by empty line
# as well as object from foreign sources
# Input:
#        $file             filen which we will filter
#        $tempfile         full filename of the temporary file
#        $ret              reference to the array in which we store:
#                          comments which go to the report (^#)
#                          dbupdate flag string (not expanded)
#        $source           source to load into
#
# Return: 1, dies if failed
# Output: nothing except from usual error messages
sub filter($$$$) {
    my $file     = $_[0];
    my $tempfile = $_[1];
    my $ret      = $_[2];
    my $source   = $_[3];

    open( FILE, "< $file" )
        or error( 'E_FOPEN', $file, $! );
    open( TEMP, "> $tempfile" )
        or error( 'E_FOPEN', $tempfile, $! );

    #my $flags = "";

    my $object = "";
    while (<FILE>) {
        if (/^#/o) {
            # save comments for report
            push @{$ret}, $_;
        }
        elsif (/^%/o) {
            # skip comments
            next;
        }
        elsif (/^\?([-]?[0-9]+)/o) {
#            setvar('DBUPDATE_IGNORE_EXIT_CODE',$1);
            next;
        }
        elsif (/^@/o) {
            # skip script line @
            next;
        }
        elsif (/^\$/o) {
            # save dbupdate flags for update
            #  if ($flags =~ /^$/) {
            #    $flags = $_;
            #  } else {
            #    s/^\$\$//;
            #    $flags = $flags.$_;
            #  }
        }
        elsif (/^[\s]*$/o) {
            if ( defined $source && ( $object =~ /^source:\s*$source\s*/im ) )
            {
                print TEMP $object, "\n\n";
                $object = "";
            }
            else {
                # just copy the blank line
                print TEMP "\n";
            }
        }
        else {    # non-empty line
            if ( defined $source ) {    #the source is set, collect the object
                $object = $object . $_;
            }
            else {
                print TEMP $_;
            }
        }
    }
    # just in case
    print TEMP "\n";

    #$flags = getvar('DBUPDATE_FLAGS') || getvar('DBUPDATE_FLAGS_EXT');

    close(FILE)
        or error( 'E_FCLOSE', $file, $! );
    close(TEMP)
        or error( 'E_FCLOSE', $tempfile, $! );

    #push @{$ret}, $flags if (defined $flags);
    return (1);
}

# Function: error()
# Descr: this is the main error reporting function
# Args: @_     array of the arguments which are passed directly to report().
#              these are error message itself and the details, in printf style.
#
# Return: nothing, but dies whith this error message (not expanded yet).
# Output: nothing, except from the one from report() function.
sub error(@) {

    # report the error
    report(@_);
    # die with this error message
    my $error = $_[0];
    $error =~ s/^(E_.*)$/$ERR->{$1}/;
    ++$CONFIG->{'ERRCOUNT'};
    my $tmp = sprintf(@_);
    if ( defined getvar('ERROR') ) {
        if ( getvar('ERRCOUNT') > getvar('ERROR') ) {
            die 'Fatal error counter exceeded';
        }
    }
    die $tmp;

}

# Function: run_script()
# executes external program
sub run_script($$) {
    my $which   = $_[0];
    my $objects = $_[1];
    my $script;
    my $test = getvar('TEST');

    #open (TESTFILE, "< $test")
    #  or error("E_FOPEN", $test, $!);

    #while (<TESTFILE>) {
    #  if (/^@/o) {
    #    $script = $_;
    #    $script =~ s/^@[\s]*//;
    #  }
    #}
    #close (TESTFILE)
    #    or error("E_CLOSE", $test, $!);

    $script = getvar($which);

    return if ( !$script );

    $script =~ s/\$(\w+)/defined($CONFIG->{$1})?$CONFIG->{$1}:''/eg;
    if ( getvar('TRACE_MAKEDB') ) {
        print ' >>> ' . $which . ': ' . $script;
    }

    use IPC::Open3;
    use IO::Select;
    $SIG{'CHLD'} = 'DEFAULT';

    my @args = split( '[\s]+', $script );
    my $command = splice( @args, 0, 1 );
    my $pid = open3( *HIS_IN, *HIS_OUT, *HIS_ERR, $command, @args );

    close(HIS_IN)
        or error( 'E_COMMRUN', $script, $! );

    my $selector = IO::Select->new();
    $selector->add( *HIS_OUT, *HIS_ERR );

    my $errfile = getvar('STDERRLOG');
    open( STDE, "> $errfile" )
        or error( 'E_FOPEN', $errfile );
    my $outfile = getvar('STDOUTLOG');
    open( STDO, "> $outfile" )
        or error( 'E_FOPEN', $outfile );

    while ( my @ready = $selector->can_read ) {
        foreach my $fh (@ready) {
            if ( fileno($fh) == fileno(HIS_ERR) ) {
                my $string = scalar <HIS_ERR>;
                if ($string) {
                    print STDE $string;
                    if ( getvar('TRACE_MAKEDB') ) {
                        print $string;
                    }
                }
            }
            else {
                my $string = scalar <HIS_OUT>;
                if ($string) {
                    print STDO $string;
                    if ( getvar('TRACE_MAKEDB') ) {
                        print $string;
                    }
                }
            }
            $selector->remove($fh) if eof($fh);
        }
    }
    my $kid;
    do {
        $kid = waitpid( $pid, 0 );
    } until ( $kid > 0 );

    # See if the script/program has received an interrupt signal. If so,
    # we will set $received_interrupt global var to be able to quit later
    # peacefully
    my $signal = ( $? & 127 );
    if ( $signal == $signo{'INT'} ) {
        $received_interrupt = 1;
    }

    if ( $? > 0 )
    {    # I know this is kinda stupid, but I couldn't do it any other way
        foreach my $entry ( @{$objects} ) {
            $entry->{"scriptdiag"} = "Errrr";
            print "ERROR: " . $which . " returned non-zero value!";
            last;
        }
    }

    close(HIS_OUT)
        or error( 'E_COMMRUN', $script, $! );
    close(HIS_ERR)
        or error( 'E_COMMRUN', $script, $! );
    close(STDE);
    close(STDO);

}

# Function clean_logs()
sub clean_logs() {
    my $logstring = uc( getvar('LOGSTRING') );
    # exclude summary which should not be deleted after every test inone batch
    $logstring =~ s/summary//i;
    my @logs = split( '[\s]+', $logstring );
    foreach my $log (@logs) {
        unlink getvar( $log . "LOG" ) if ( -e getvar( $log . "LOG" ) );
    }

    # unlink the debug file if exists
    my $rotated = getvar('CURRENT_DIR');
    $rotated =~ s/\//_/og;
    $rotated = getvar('DUMPDIR') . "/$rotated.DEBUG";
    unlink($rotated) if ( -e $rotated );

}

# this will add current paths to every local file
sub init_paths() {
    my $dir = getvar('CURRENT_DIR');

    foreach (qw/ TEST DBUPDATE_FILE FILTERS_LOCAL LOADER_FILE /) {
        my $tmp = getvar($_);
        # save the template if 1st time
        setvar( $_ . "_TEMPLATE", $tmp ) if ( !getvar( $_ . "_TEMPLATE" ) );
        # set new var
        setvar( $_, "$dir/" . getvar( $_ . "_TEMPLATE" ) );
    }

}

##############################
# Function: run_test()
# Descr: run the test in specified directory
# Input: $filters             reference to generic filters hash
#                             of hashes. each hash in form attr=>value
# Return: result - OK or FAILED or error identifier
# Output: error report and underlying function messages
sub run_test($) {
    my $filters = $_[0];
    my $dir     = getvar('CURRENT_DIR');

    my @vars_cleanup
        = ( 'DBUPDATE_FLAGS', 'DBUPDATE_FLAGS_EXT', 'EXEC_BEFORE',
        'EXEC_AFTER', 'QUERY_AF', 'TEST_RIR', 'DBUPDATE_IGNORE_EXIT_CODE' );

    # initialize filenames
    init_paths();
    # get and set the per-test variables from the 'test' file

    set_test_variables();

#### check if we need to run the test

    my $result = "FAILED";

    # 0.a clean old logs
    clean_logs();
    stamp_test($dir);

    # 00. TRACING

    trace();

    # 0. print header

    report("\n\n<<< Start of Test Run >>>");

    report( "\n%% Running test in directory %s\n", $dir );
    report( "%% Started: %s",                      `date` );
    report( "%% Username: %s\n",                   scalar( getpwuid($>) ) );

    # 1. parse local filters, die if not successful.
    my $all = parse_filters( getvar('FILTERS_LOCAL') );

    my $objects       = $$all[0];
    my $local_filters = $$all[1];

# 2. save references to generic filters to the hash with custom filters and resolve

    for ( keys %{$filters} ) {
        if ( !exists $local_filters->{$_} ) {
            $local_filters->{$_} = $filters->{$_};
        }
        else {
            error( "E_DUPFLTR", $_ );
        }
    }

    #
    reference_check( $objects, $local_filters );

    # clear revocation list + copy over the "=" file from test dir
    # do_crl();

    # 3. empty + load database + dbupdate initial run + log cleanup

    timeout( 'E_TIME', 'make_db', \&make_db );

    # 4. run script before object
    timeout( 'E_TIME', "external script", \&run_script, "EXEC_BEFORE",
        $objects );

    # 5. submit objects
    my $testfile = getvar('TEST');
    timeout( 'E_TIME', 'dbupdate', \&run_update, getvar('TEST'), 0 );

    # 6. match filters
    my @logs = split( '[\s]+', getvar('LOGSTRING') );
    foreach my $log_type (@logs) {
        my $logname = getvar( uc($log_type) . "LOG" );
        # catch fatal errors and return immediately if any
        match_filter( $objects, $local_filters, $log_type, $logname );
        # if it fails with particular log type, we should go further.
        #match_filter_exp($objects, $local_filters, $log_type, $logname);
    }

# 7. match whois output with the one expected and add diagnostics to the object
    timeout( 'E_TIME', 'whois', \&match_whois, $objects, $local_filters );

    # 7a. match query output with one expected
    timeout( 'E_TIME', 'query', \&match_query, $objects, $local_filters );

    # 7b. execute external command - any error is fatal!
    timeout( 'E_TIME', "external script", \&run_script, "EXEC_AFTER",
        $objects );

    # 8. unset the variables
    foreach my $var (@vars_cleanup) {
        delvar($var);    # if (getvar($var));
    }

    # 9. print report
    $result = print_report($objects);

    # 10. return result
    return ($result);

}
##############################
# Function: parse_command()
# Descr: parse command line and prepare for running
# Input: @ARGV
# Return:  reference to $CONFIG hash or undef if failed. For hash format, wee above.
# Output: some defaults are set.
sub parse_command() {

    # DEFAULTS END

    my $opt;
    $opt = GetOptions(
        "trace_test"     => \$CONFIG->{"TRACE_TEST"},
        "t"              => \$CONFIG->{"TRACE_TEST"},
        "trace_dbupdate" => \$CONFIG->{"TRACE_DBUPDATE"},
        "d"              => \$CONFIG->{"TRACE_DBUPDATE"},
        "trace_makedb"   => \$CONFIG->{"TRACE_MAKEDB"},
        "m"              => \$CONFIG->{"TRACE_MAKEDB"},
        "output_report"  => \$CONFIG->{"OUTPUT_REPORT"},
        "o"              => \$CONFIG->{"OUTPUT_REPORT"},
        "config=s"       => \$CONFIG->{"CONFIG"},
        "c=s"            => \$CONFIG->{"CONFIG"},
        "limit=i"        => \$CONFIG->{"LIMIT"},
        "l=i"            => \$CONFIG->{"LIMIT"},
        "errors=i"       => \$CONFIG->{"ERROR"},
        "z=i"            => \$CONFIG->{"ERROR"},
        "stderr"         => \$CONFIG->{"STDERR"},
        "e"              => \$CONFIG->{"STDERR"},
        "stdout"         => \$CONFIG->{"STDOUT"},
        "u"              => \$CONFIG->{"STDOUT"},
        "rundir=s"       => \$CONFIG->{"RUNDIRS"},
        "r=s"            => \$CONFIG->{"RUNDIRS"},
    );

    # delete "unset options"
    for ( keys %{$CONFIG} ) {
        delete $CONFIG->{$_} if ( !defined $CONFIG->{$_} );
    }

    # if errors in GetOptions
    if ( !%{$CONFIG} || !$opt || ( $CONFIG->{"RUNDIRS"} !~ /^\// ) ) {
        print $ERR->{'E_COMLINE'};
        print "
    usage:
    --trace_test, -t      turn on test tracing
    --trace_dbupdate, -d  turn on dbupdate tracing 
    --trace_makedb, -m    display output of make_db_test
    --output_report, -o   print report to stdout
    --stderr, -e          print dbupdate stderr to stderrlog
    --stdout, -u          print dbupdate stdout to stdoutlog
    --config, -c          configuration file
    --limit, -l        limit of mismatches (failed tests)
    --errors, -z       limit of errors (runtime errors)
    --rundir, -r          <list of directories separated by space, absolute paths>\n\n";
        return;
    }

    return ($CONFIG);

}

##############################
# Function: report()
# Descr: writes the line into REPORTLOG; if -o, also to /dev/tty
# Input: @_ containing:
#       $format            printf-style formatted string
#       @args              list of arguments
# Return: nothing
# Output: the string into log file
sub report(@) {
    my $format = shift(@_);
    my @args   = @_;

    my $reportlog = getvar('REPORTLOG');

    open( REPORTLOG, ">> $reportlog " )
        or die "Can't open file $reportlog: $!";

    $format =~ s/(E_\S*)/defined($ERR->{$1})?$ERR->{$1}:$1/ie;
    $format =~ s/%([HMS])/%%$1/g;

    printf REPORTLOG $format, @args;
    printf STDOUT $format, @args if ( getvar('OUTPUT_REPORT') );
    close(REPORTLOG)
        or die "Can't close file $reportlog: $!";

}

sub stamp_test($) {
    # to 'dir-stamp' the test output in a log file

    my $dir       = $_[0];
    my $logstring = getvar('LOGSTRING');
    my @logs      = split( '[\s]+', $logstring );
    foreach my $log (@logs) {
        if ( $log =~ /report/i ) {
            next;
        }
        $log = uc($log);
        $log .= "LOG";
        my $tolog = getvar($log);

        open( LOG, ">> $tolog " )
            or die "Can't open file $tolog: $!";

        print LOG "<<< TEST RUN $dir >>>\n";
        #print STDOUT "DEBUG <<< printing to $tolog >>>\n";

        close(LOG)
            or die "Can't close file $tolog: $!";

    }
}

sub is_ignored ($) {
    # returns true if dir should be ignored, and false otherwise
    my @ignore_dirs = (qw /CVS doc README/);
    my $dir         = $_[0];

    foreach (@ignore_dirs) {
        return 1 if ( ( $dir =~ /\/$_[\s]*\/?$/ ) || ( $dir =~ /^(#|%)/ ) );
    }
    return 0;

}
##############################
# Function: get_test_dirs()
# Descr: composes the lists of directories where we run and the ones which we skip.
# Input: nothing
# Return: reference to array containing:
#         reference to array with dirs we run
#         reference to array with dirs we skip
# Output: none
# Remarks: here expand_dir function is recursive and used to retrieve the directory trees of indefinite depth

sub get_test_dirs() {
    my @exclude_dirs = ();
    my $excludefile  = getvar('EXCLUDE');
    my $datadir      = getvar('DATADIR');

    # find the test which should be excluded (not mandatory).
    if ( defined $excludefile ) {
        open( EXCLUDE, "< $excludefile" )
            or die "Cannot open file $excludefile: $!";
        while (<EXCLUDE>) {
            chomp();
            s/\/$//;
            s/^/$datadir\//;
            push @exclude_dirs, expand_dir($_) if ( !is_ignored($_) );
        }
        close(EXCLUDE)
            or die "Couldn't close $!";
    }

    my @test_dirs;
    my $rundirs = getvar('RUNDIRS');

    if ( defined $rundirs ) {
        my @dirs = split( '[\s]*,[\s]*', $rundirs );
        foreach my $dir (@dirs) {
            $dir =~ s/\/$//;
            unless ( -e $dir ) {
                error( 'E_DIR', $dir );
            }
            push @test_dirs, expand_dir($dir) if ( !is_ignored($dir) );
        }
    }
    else {
        # DEFAULT - all
        my $datadir = getvar('DATADIR');
        my @dirs    = glob("$datadir/*");
        foreach my $dir (@dirs) {
            push @test_dirs, expand_dir($dir) if ( !is_ignored($dir) );
        }
    }

    my @run;
    my @skipped;
    foreach my $dir (@test_dirs) {
        my $flag;
        if (@exclude_dirs) {
            foreach my $exclude_path (@exclude_dirs) {
                if ( $dir =~ /^$exclude_path(($)|(\/.*?$))/i ) {
                    push @skipped, $dir;
                    $flag = 1;
                }
            }
        }
        if ( is_ignored($dir) ) {
            # ignore if needed
        }
        elsif ( !$flag ) {
            push @run, $dir;
        }
    }
    my @all = ( \@run, \@skipped );

    if ( getvar('TRACE_TEST') ) {
        print( "TEST_TRACE: Tests to run: ", scalar(@run), "\n" );
        foreach my $t (@run) {
            print("TEST_TRACE: RUN $t\n");
        }
        print( "TEST_TRACE: Tests to skip: ", scalar(@skipped), "\n" );
        foreach my $t (@skipped) {
            print("TEST_TRACE: SKIP $t\n");
        }
    }

    return ( \@all );

}

##############################
# Function: expand_dir()
# Descr: expands the directory name into the 'runnable' test directories
# Input: $name                the absolute directory name where
#                              we'll search for useful directories
#
# Return: arrary of dirs where we actually will _run_ the test.
# Output: nothing
# Remark: recursion!!!
sub expand_dir($) {
    my $name = $_[0];
    my @expands;

    if ( -d $name ) {
        my @dirs = glob("$name/*");
        foreach my $dir (@dirs) {
            if ( !is_ignored($dir) ) {
                if ( -d $dir ) {
                    my @d;
                    @d = &expand_dir($dir);
                    push @expands, @d;
                }
                else {
                    push @expands, $name
                        if ( join( ' ', @expands ) !~ /$name/ );
                    next;
                }
            }
        }
        return (@expands);
    }
}

########################
# Function print_summary()
# Descr: prints the summary to the summarylog file, along with some stats.
# Input: reference to array containing
#        1. ref. to array of tested dirs
#        2. ref. to array of skippied dirs
#        3. ref. to hash with results (in form dirname=>"OK|FAILED")
#        4. ref. to array with successful (test) dirs
#        5. ref. to array with failed (test) dirs
# Output: contents of the summarylog

sub print_summary($) {

    my $dirs = $_[0];

    my $test_dirs = $$dirs[0];
    my $skip_dirs = $$dirs[1];
    my %results   = %{ $$dirs[2] };
    my $ok        = $$dirs[3];
    my $failed    = $$dirs[4];

    my $summarylog = getvar('SUMMARYLOG');
    open( SUMMARY, "> $summarylog" )
        or error( "E_FOPEN", $summarylog, $! );

    print SUMMARY "% Finished: ";
    print SUMMARY `date`;
    print SUMMARY "% Username: ";
    print SUMMARY $ENV{USER};
    print SUMMARY "\n";
    print SUMMARY "% TESTS RUN:     ", scalar( @{$test_dirs} ), "\n";
    print SUMMARY "% TESTS SKIPPED: ", scalar( @{$skip_dirs} ), "\n";
    print SUMMARY "% TESTS OK:      $ok\n";
    print SUMMARY "% TESTS FAILED:  $failed\n\n";
    if ( $failed != 0 ) {
        for ( sort ( keys %results ) ) {
            print SUMMARY "% FAILED: $_\n" if ( $results{$_} eq "FAILED" );
        }
        print SUMMARY "\n";
    }

    print SUMMARY "% Test dirs: \n";
    foreach ( @{$test_dirs} ) {
        print SUMMARY "% $_\n";
    }
    print SUMMARY "\n% Skipped dirs: \n";
    if ($skip_dirs) {
        foreach ( @{$skip_dirs} ) {
            print SUMMARY "% $_\n";
        }
    }

    print SUMMARY "% \n";

    print SUMMARY "<<< End of Test Run >>>\n";
    close(SUMMARY)
        or error( 'E_FCLOSE', $summarylog, $! );
}

#####################
# Function timeout()
# Descr: used to timeout the operation
# Args: array containing:
#      1. $message        which message to print out if operation timed out
#      2. $commandname    what we will be running (human-readable name)
#      3. $operation      link to the function to run
#      4. @args           array to store all arguments to the function
# Output: error messages if any

sub timeout(@) {
    my $message     = shift(@_);
    my $commandname = shift(@_);
    my $operation   = shift(@_);
    my @args        = @_;

    eval {
        $SIG{'ALRM'} = sub { die "timeout"; };
        alarm(600);
        $operation->(@args);
        alarm(0);
    };

    if ($@) {
        if ( $@ =~ /timeout/ ) {
            error( $message, $commandname );    # give a message and die
        }
        else {
            alarm(0);
            #error already reported
            die;
        }
    }
}

##############
# Function do_crl()
# Descr: clear directory CRLLIST from rip.config;
#        copy any file named "=*" from test dir to CRLLIST directory (if any file exists)
# check all exit codes
# Args: none
# Output: error message if any

sub do_crl() {
    my $crldir   = getvar('CRLLIST');
    my $wildcard = getvar('CURRENT_DIR') . "/*=*";

    # 1. clear the directory
    my @list = glob("$crldir/*");
    foreach (@list) {
        if ( -f $_ ) {
            unlink($_);
        }
    }

    # 2. copy local files to that dir if any
    @list = glob($wildcard);
    if ( ( scalar @list ) != 0 ) {
        foreach my $name (@list) {
            my $from_file = $name;
            $name =~ s/^(.*)\///g;
            my $to_file = "$crldir/$name";
            open( TO, "> $to_file" )
                or error( "E_FOPEN", $to_file, $! );
            open( FROM, "< $from_file" )
                or error( "E_FOPEN", $from_file, $! );
            while (<FROM>) {
                print TO $_;
            }
            close(FROM);
            close(TO);
        }
    }
}

##############
## Function get_signal_numbers()
## Descr: Gets a list of signal numbers and names, as we will need
##     the number of INT signal of the system on which we run
## Args: none
## Output: Writes into global hashes called @signame and %signo
#
sub get_signal_numbers {
    use Config;
    defined $Config{sig_name} || die "Could not gather signal numbers!";
    my $i = 0;    # Config prepends fake 0 signal called "ZERO"
    foreach my $name ( split( ' ', $Config{sig_name} ) ) {
        $signo{$name} = $i;
        $signame[$i] = $name;
        $i++;
    }
}

##########################################################################
#
# BEGIN MAIN
# test.pl
# usage:
# --trace_test, -t 			turn on test tracing
# --trace_dbupdate, -d		turn on dbupdate tracing
# --output_report, -o	    print report to stdout
# --source, -s 				consider this source while testing
# --config, -c              configuration file
# --stderr, -e        collect stderr from dbupdate
# --stdout, -u        collect stdout from dbupdate
# --errors, -z        limit to hard errors
# --limit, -l        limit to test mismatches
# --rundir, -r              <list of directories sep. by space>, recursive.
#
##########################################################################
{

    $SIG{'QUIT'} = $SIG{'STOP'} = $SIG{'TERM'} = sub { die 'signal'; };
    $SIG{'INT'} = sub { $received_interrupt = 1; };

    # set signal numbers & names first, to be used later
    &get_signal_numbers;

    eval {

        # for all test sets

        # parse command line and configuration file
        die('Bad configuration') if ( !parse_command() );
        die('Bad configuration') if ( !parse_config() );

# spent half day tracking down this - won't allow that happen to anybody again - agoston, 2006
        die('You MUST set -e flag, or some of the tests will fail!')
            if ( !getvar('STDERR') );
        die('You MUST set -o flag, or some of the tests will fail!')
            if ( !getvar('STDOUT') );

        # set up the log file names - fully qualified
        sleep(20) if ( strftime( "%H%M%S", localtime ) >= 235945 );

        my $date = strftime( "%Y%m%d", localtime );
        my @logs = split( '[\s]+', getvar('LOGSTRING') );

        foreach my $log_type (@logs) {
            $log_type = $log_type . "log";
            $log_type = uc($log_type);
            my $logfile = getvar($log_type) . ".$date";
            setvar( $log_type, $logfile );
            # dumping
            my $varname = "DUMP" . $log_type;
            $log_type = lc($log_type);
            $logfile  = getvar("DUMPDIR") . "/$log_type.$date";
            setvar( $varname, $logfile );
        }

        # prepare aux directories
        mkdir getvar("DUMPDIR")
            || die "Can't create directory: getvar(\"DUMPDIR\")"
            unless ( -x getvar("DUMPDIR") );
        mkdir getvar("LOGDIR")
            || die "Can't create directory: getvar(\"LOGDIR\")"
            unless ( -x getvar("LOGDIR") );
        mkdir getvar("TEST_TMPDIR")
            || die "Can't create directory: getvar(\"TEST_TMPDIR\")"
            unless ( -x getvar("TEST_TMPDIR") );

        # fetch generic filters

        my $all     = parse_filters( getvar('FILTERS') );
        my $filters = $$all[1];

        rotate_logs(1);

        # where to run tests
        # expand test dirs

        my $dirs      = get_test_dirs();
        my $test_dirs = $$dirs[0];
        my $skip_dirs = $$dirs[1];
        my @run_dirs;

        # run! (and gather some statistics)

        my $summarylog = getvar('SUMMARYLOG');
        open( SUMMARY, "> $summarylog" )
            or error( "E_FOPEN", $summarylog, $! );

        print SUMMARY "\n\n<<< Start of Test Run >>>\n";
        print SUMMARY "% Started: ";
        print SUMMARY `date`;
        close(SUMMARY);
        #
        rotate_logs(2);

        my %results;
        my $failed  = 0;
        my $ok      = 0;
        my $skipped = 0;
        $skipped = scalar( @{$skip_dirs} ) if ($skip_dirs);
        my $limit = getvar('LIMIT');

        foreach my $dir ( @{$test_dirs} ) {
            if ( defined($limit) && ( $failed > $limit ) ) {
                error( 'E_LIMIT', $limit );
            }
            $CONFIG->{"CURRENT_DIR"} = $dir;
            my $res;
            eval { $res = run_test($filters); };
            rotate_logs(0);
            rotate_logs(1);
            remove_tmp();
            if ($@) {
                $res = "FAILED";
                #fatal errors
                report("<<< End of Test Run >>\n");
                if ( $@ =~ /timeout|limit|counter/i ) {
                    # exit abnormally
                    die $@;
                }
            }
            # check if any other error occured
            $res = "FAILED" if ( !defined($res) );
            $results{$dir} = $res;

            $failed++ if ( $res eq "FAILED" );
            $ok++     if ( $res eq "OK" );
            if ( $res eq "SKIPPED" ) {
                $skipped++;
                push @{$skip_dirs}, $dir;
            }
            else {
                push @run_dirs, $dir;
            }

        # at this point of the loop, check if we or our children have received
        # an INT signal. If so, let's quit peacefully, by exiting the loop
        # here.  The user wanted us to quit, so we quit.
            if ($received_interrupt) {
                print STDERR
                    "\nWe have received INT at some point, so quitting.\n";
                print STDERR
                    "You might need to restart your test whois server,\n";
                print STDERR
                    "as we might be quitting before issuing \"set updates resume\"\n";
                print STDERR
                    "and/or \"set queries resume\" in the administrative interface.\n\n";
                last;
            }

        }

# everything goes as 1 argument: $test_dirs, $skip_dirs, \%results, $ok, $failed, $time
# See print_summary.
# replace test_dirs with run_dirs (because some tests may have been skipped)
        $$dirs[0] = \@run_dirs;
        push @{$dirs}, ( \%results, $ok, $failed );
        print_summary($dirs);

    };    #end of eval

    if ( $@ !~ /Bad configuration/ ) {
        rotate_logs(1);
        rotate_logs(2);
    }
    if ($@) {
        die "Aborted due to fatal errors: $@\n";
    }

}

# END MAIN
