#!/usr/bin/perl

#/***************************************
#  $Revision: 1.2

#  dbtestrun.pl

#  Status: NOT REVIEWED, NOT TESTED

#  Author(s):       Magnus Karlsson

#  ******************/ /******************
#  Modification History:
#        magnus (01/10/2001) Created.
#		denis (10/12/2001) Modified
#  ******************/ /******************
#  Copyright (c) 2001                              RIPE NCC

#  All Rights Reserved

#  Permission to use, copy, modify, and distribute this software and its
#  documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and that
# both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.

#  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
#  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
#  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
#  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
#  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# ***************************************/

# Reads update messages from a given file, feeds them to two
# versions of dbupdate, compares ack and notif messages, whois
# output one message at a time.

#  while msgs {
#    get msgs;
#    run dbupdate 3.0;
#    run dbupdate 3.1;
#    compare ack;
#    compare notif;
#    compare objects (Whois);
#    }
#  compare db;

# The data file can contain any number of update objects, with or without mail headers.
# An assumption has been made that if a line of the format 
#       >>>  anything   <<<
# is read then the following update will have a mail header and the -M flag will be
# included in the call to dbupdate..
# Updates read from the data file will be seperated either by a line starting with
# two dollar signs ($$), as in the case of the standard test data, or by a line
# containing the >>>  anything   <<< pattern, as in the case of an update log file.
# The two can be mixed if required, but it would probably be more sensible to run the
# test program with the standard test data and then run it seperately with a copy of
# an update log file.

# Within the loop an update message is read from the data file.

# This message is passed to the two versions of dbupdate being run.

# The ack messages from the ack log files are compared taking the whole message as a 
# single string, but removing blank lines and the time stamp line. Ack messages should 
# always be produced for any update. If either ack log file is not found or if there is a
# difference in the two ack messages an error is logged including the ack messages.
# The ack log file is unlinked at the start of the loop so it will only ever contain the
# ack message for the current update.

# The notif messages from the notif log files are compared taking the whole message as a 
# single string, but removing blank lines and the time stamp line. Notif messages are not
# always produced for any particular update. If one version produces a notif message and
# the other version does not, or if both versions produce notif messages which are
# different, then an error is logged including the notif messages.
# The notif log file is unlinked at the start of the loop so it will only ever contain
# the notif message for the current update.

# A whois query is performed for each version. The host and query port numbers are read 
# from the rip.config file. The whole output from the whois query is taken as a single
# string and compared. If they are different an error is logged including the whois output.

# Apart from opening the testlog file, all other errors are passed to the error_check
# sub routine. This allows for a neat exit giving an error message and printing the
# counts of number of updates processed and number of updates causing errors.

# Parameters:
# Usage: <update_file> <configV2> <configV3> <errorlimit>  <trace>\n\n";

#   update_file: path to the file containing update messages to be fed to dbupdate.
#                This can be a full path name or relative to the test program.

#   configV2: old config file including full path. This must be the full path as the
#             program splits it up to get the root dir for the dbupdate binary and
#             all the log files.

#   configV3: new config file including full path. This must be the full path as the
#             program splits it up to get the root dir for the dbupdate binary and
#             all the log files.

#   errorlimit: number of updates causing errors before quitting (optional, default 1)
#               This value set the number of non-fatal errors that can be found before
#               the test program will terminate.

#   trace: 1 to set the trace flag, 0 for no trace (optional, default 0)
#          If set the -t flag will be included in the call to both dbupdates.




use Time::localtime;


# read command line arguments

if ( $#ARGV != 2 && $#ARGV != 3 && $#ARGV != 4 )
  {
    print "Usage: <update_file> <configV2> <configV3> <errorlimit>  <trace>\n\n";
    print "   update_file: path to the file containing update messages to be fed to dbupdate\n";
    print "   configV2: old config file including full path\n";
    print "   configV3: new config file including full path\n";
    print "   errorlimit: number of updates causing errors before quitting (optional, default 1)\n";
    print "   trace: 1 to set the trace flag, 0 for no trace (optional, default 0)\n";
    exit 1;
  }

$Mail_message = 0;		# set if current update is a mail message
$Update_count = 0;		# number of updates processed
$Error_count = 0;		# number of update messages that have caused errors
$First_error = 1;		# set if it is the first error caused by current update
$Up_file_name = "update_file.txt";  # temp file used to store current update to pass to dbupdate
$HostV2;
$PortV2;
$HostV3;
$PortV3;
$PathV2 = NULL;
$PathV3 = NULL;
$ConfigV2 = $ARGV[1];
$ConfigV3 = $ARGV[2];

$no_whois_params = 0;
my $tm = localtime;
$today = sprintf("%04d%02d%02d", $tm->year+1900,($tm->mon)+1, $tm->mday);
 
# set the error limit - number of updates causing errors before the program will die.
if ( defined($ARGV[3]) )
{
  $Err_limit = $ARGV[3];
}
else
{
  $Err_limit = 1;
}

# set the trace flag.
if ( defined($ARGV[4]) )
{
  $Set_trace = $ARGV[4];
}
else
{
  $Set_trace = 0;
}

$PathV2 = $ConfigV2;
$PathV2 =~ s#/conf/.*$#/# ;
$PathV3 = $ConfigV3;
$PathV3 =~ s#/conf/.*$#/# ;
#$Up_file_name = $PathV3."log/test/$Up_file_name ";

open (TESTLOG, ">$PathV3"."log/test/dbupdatetest.$today")
  or die "Cannot open test log file $PathV3"."log/test/dbupdatetest.$today ($!)\n";

open (DATAFILE, "$ARGV[0]")
  or error_check( "Cannot open data file $ARGV[0] ($!)", "fatal");

if ( ! get_whois3_params("V2") )
{
  print TESTLOG "\nwhois3 host and port not found in $ConfigV2\n Continuing test without whois checks\n";
  $no_whois_params = 1;
}
if ( ! get_whois3_params("V3") )
{
  print TESTLOG "\nwhois3 host and port not found in $ConfigV3\n Continuing test without whois checks\n";
  $no_whois_params = 1;
}

while ( get_update(DATAFILE) )
{
  $Update_count++;
  &old_delete($PathV2);
  &old_delete($PathV3);

  if ( $Error_count == $Err_limit )
  {
    error_check("Maximum number of errors encountered", "error level reached" );
  }

  $First_error = 1;
  &feed_update();

  $lookupkey = &ack_compare();

  &notif_compare();

  if ( $lookupkey ne "" && ! $no_whois_params )
  {
    &whois_compare($lookupkey);
  }

  $First_error = 1;		#reset for next update message
#  unlink $Up_file_name;
}

error_check("Test run completed", "end");

close(DATAFILE);
close(TESTLOG);

# MAIN PROGRAM ENDS HERE


sub get_update
{
  # get the next update from the data file and write it to the update file
  my $datafn = shift;
  
  # first delete the update file if it exists, then open it.
  if ( -e "$PathV3"."log/test/"."$Up_file_name" )
  {
	unlink "$PathV3"."log/test/"."$Up_file_name" 
      or error_check( "Can not delete the update_file $PathV3"."log/test/"."$Up_file_name. $!.", "fatal" )
  }
  open(UPDATEFILE, ">$PathV3"."log/test/"."$Up_file_name") 
    or error_check(  "Cannot open update file $PathV3"."log/test/"."fred ($!)", "fatal" );

  # now read the next update from the data file and write it to the update file
  $linecnt = 0;
  $Mail_message = 0;
  while ( $line = <$datafn> )
  {
    if ( $line =~ /^\$\$/ )
	{
	  last;   # $$ defines end of update record in standard test data sets.
	}

    if ( $line =~ /^##/ )
	{
	  next;   # ## defines a comment line in standard test data sets.
	}
	
	if ( $line =~ /^>>>.*<<<$/ )
	{
	  # denotes an email update message
	  # if this is the first line we have read (ignoring preceeding blank lines)
	  # then it is the start of the update we are reading in.
	  if ( $linecnt == 0 )
	  {
	    $Mail_message = 1;    # set the -M flag for dbupdate
		next;                 # no point adding this line to the update file
	  }
	  else
	  {
	    # we have already read some lines, so this must be the start of the next update
		# rewind the file pointer so that we read this line again at the start of the 
		# next call to get_update.
		$blen = length $line;
		seek($datafn, -$blen, 1);   # move back $blen bytes from current position
		last;
	  }
	}
	
	if ( $line =~ /^\s*\n/ ) 
	{
	  # blank line
	  if ( $linecnt == 0 )
	  {
	    next;       # ignore blank lines at the start of an update message
	  }
	}
	
	# otherwise add the line to the update file
	print UPDATEFILE $line;
	$linecnt++ ;
  }
  
  close UPDATEFILE;
  
  if ( $linecnt == 0 && ! defined($line) )
  {
    return 0;   # we have not read an update and reached the end of the data file
  }
  else
  {
    return 1;   # we have an update to process.
  }
}

# Deletes old log files if they exist

sub old_delete {
  my $path = shift(@_);

  my $tm = localtime;
  $today = sprintf("%04d%02d%02d", $tm->year+1900,($tm->mon)+1, $tm->mday);

  if (-e "$path"."log/ack/acklog.$today" && -e "$path"."log/notif/notiflog.$today") 
  {
    error_check( "Found old log files but could not delete them. $!.", "fatal" )
      if (unlink ("$path"."log/ack/acklog.$today", "$path"."log/notif/notiflog.$today") != 2);
  }
  elsif (-e "$path"."log/ack/acklog.$today" || -e "$path"."log/notif/notiflog.$today") 
  {
    error_check( "Found old log files but could not delete them. $!.", "fatal" )
      if (unlink ("$path"."log/ack/acklog.$today", "$path"."log/notif/notiflog.$today") != 1);
  }
}


###################################################################
######
##### Takes a given file and feeds it into two versions of dbupdate
#### The paths to the different dbupdates are given as the second
### and third argument.
## uses high-precision timing from the Perl Cookbook.
#

sub feed_update
{
    # required for syscall calls, when timing the processes
    require 'sys/syscall.ph';

    # initialize the structures returned by gettimeofday
    my $TIMEVAL_T = "LL";
    my $done1 = my $start1 = pack($TIMEVAL_T, ());
    my $done2 = my $start2 = pack($TIMEVAL_T, ());
	my $processV2 = NULL;
	my $processV3 = NULL;

    # read the time into $start1
    syscall((&SYS_gettimeofday, $start1, 0)) != -1
      || {$start1 = ""};

    # feed argument to the V2 database, with -M option if $Mail_message is set
    # -S supress acknowledgement mails

    if ($Mail_message)
	{
	  $mailflag = "-M";
	}
	else
	{
	  $mailflag = "";
	}
	  
	if ($Set_trace)
	{
	  $traceflag = "-t";
	}
	else
	{
	  $traceflag = "";
	}
    system("$PathV2"."bin/dbupdate -S $mailflag $traceflag -c $ConfigV2 -f $PathV3"."log/test/"."$Up_file_name 2>/dev/null");

    # save the return value of the process
	$processV2 = $? ;

    # read the time into $done1
    syscall(&SYS_gettimeofday, $done1, 0) != -1
      || {$done1 = ""};

    # read the time into $start2
    syscall((&SYS_gettimeofday, $start2, 0)) != -1
      || {$start2 = ""};

    # feed argument to the V3 database, with -M option if $Mail_message is set
    # -S supress acknowledgement mails

    system("$PathV3"."bin/dbupdate -S $mailflag $traceflag -c $ConfigV3 -f $PathV3"."log/test/"."$Up_file_name 2>/dev/null");

    # save the return value of the process
	$processV3 = $? ;

    # read the time into $done2
    syscall(&SYS_gettimeofday, $done2, 0) != -1
      || {$done2 = ""};

    $process_msg = "" ;
	if ( $processV2 != 0 )
	{
	  $process_msg .= "Abnormal termination of dbupdate V2: Exit value: " . ($processV2 >> 8) . " Signal: " . ($processV2 & 127) . "\n";
    }
	if ( $processV3 != 0 )
	{
	  $process_msg .= "Abnormal termination of dbupdate V3: Exit value: " . ($processV3 >> 8) . " Signal: " . ($processV3 & 127) . "\n";
    }
    if ( $process_msg != "" )
	{
	  error_check($process_msg, "continue");
	}

	if ( $start1 != "" && $done1 != "" && $start2 != "" && $done2 != "" )
	{
      # expand the structures
      my @start1 = unpack($TIMEVAL_T, $start1);
      my @start2 = unpack($TIMEVAL_T, $start2);
      my @done1  = unpack($TIMEVAL_T, $done1);
      my @done2  = unpack($TIMEVAL_T, $done2);

      # fix microseconds
      for ($done1[1], $done2[1], $start1[1], $start2[1])
	  {
    	$_ /= 1_000_000;
      }

      # calculate time difference
      my $delta_time1 = sprintf "%.4f", ($done1[0]  + $done1[1]  )
    	- ($start1[0] + $start1[1] );

      my $delta_time2 = sprintf "%.4f", ($done2[0]  + $done2[1]  )
    	- ($start2[0] + $start2[1] );

      print "V3 took $delta_time1 seconds\n";
      print "V2 took $delta_time2 seconds\n";
	}
}

###################################################################
#####
#### Reads the acklogs from two directories, comparing them
### for differences and returns the lookup-key.
## 
#

sub ack_compare
{
    my $tm = localtime;
    my $lookup_key = "";
	my @bits;
    my $msgV2 = "";
    my $msgV3 = "";

    # get today's date in YYYYMMDD format
    $today = sprintf("%04d%02d%02d", $tm->year+1900,($tm->mon)+1, $tm->mday);

    # acklog assumed to be in the subdirectory ack/

    if (-e "$PathV2"."log/ack/acklog.$today") 
	{
      open (ACKFILE_V2, "$PathV2"."log/ack/acklog.$today")
    	or error_check( "Cannot open $PathV2"."log/ack/acklog.$today ($!)", "fatal" );
      foreach(<ACKFILE_V2>)
      {
		unless ( /^\s*\n/ || /^>>>.*<<<$/ )
		{
	      $msgV2 .= $_;
		}
      }
      close (ACKFILE_V2);
	}
	else
	{
	  error_check( "ack log file $PathV2"."log/ack/acklog.$today not found($!)", "continue" );
	}

    if (-e "$PathV3"."log/ack/acklog.$today") 
	{
      open (ACKFILE_V3, "$PathV3"."log/ack/acklog.$today")
    	or error_check( "Cannot open $PathV3"."log/ack/acklog.$today ($!)", "fatal" );;
      foreach(<ACKFILE_V3>)
      {
		unless ( /^\s*\n/ || /^>>>.*<<<$/ )
		{
	      $msgV3 .= $_;
		}

		if (/^[New|Update].+\[/) 
		{
          # extract lookup-key from acknowledgement line
	      # grab what's behind the object name in brackets, should be a nic-hdl
	      @bits = split /\]/;
	      $lookup_key = $bits[1];
		}
      }
      close (ACKFILE_V3);
	}
	else
	{
	  error_check( "ack log file $PathV3"."log/ack/acklog.$today not found($!)", "continue" );
	}

    unless ($msgV2 eq $msgV3) 
	{
	  error_check("*** ERROR: Different ack messages!\n*** Version 2 ack message:\n\n$msgV2\n\n*** Version 3 ack message:\n\n$msgV3", "continue");
	}

    if ( $lookup_key eq "" )
	{
	  error_check("*** ERROR: No lookup key found for whois query", "continue");
	}

	return $lookup_key;
}


###################################################################
#####
#### Reads the notiflogs from two directories, returns 1
### if one version produced notif message but the other
## didn't or if they are different.
#


sub notif_compare
{
    my $tm = localtime;
	my $nolog_V2 = 0;
	my $nolog_V3 = 0;
    my $msgV2 = "";
    my $msgV3 = "";

    # get today's date in YYYYMMDD format
    $today = sprintf("%04d%02d%02d", $tm->year+1900,($tm->mon)+1, $tm->mday);

    open (NOTIFFILE_V2, "$PathV2"."log/notif/notiflog.$today")
      or $nolog_V2 = 1;
    open (NOTIFFILE_V3, "$PathV3"."log/notif/notiflog.$today")
      or $nolog_V3 = 1;
	
	if ( $nolog_V2 != $nolog_V3 )
	{
	  if ( $nolog_V2 )
	  {
        close (NOTIFFILE_V3);
	    error_check("*** ERROR: old version has not created the notiflog file $PathV2"."log/notif/notiflog.$today", "continue");
	    return 1;
	  }
	  else 
	  {
        close (NOTIFFILE_V2);
	    error_check("*** ERROR: new version has not created the notiflog file $PathV3"."log/notif/notiflog.$today", "continue");
	    return 1;
	  }
	}

    if ( ! $nolog_V2 && ! $nolog_V3 )
	{
      foreach(<NOTIFFILE_V2>)
      {
		unless ( /^\s*\n/ || /^>>>.*<<<$/ )
		{
	      $msgV2 .= $_;
		}
      }

      foreach(<NOTIFFILE_V3>)
      {
		unless ( /^\s*\n/ || /^>>>.*<<<$/ )
		{
	      $msgV3 .= $_;
		}
      }

      close (NOTIFFILE_V2);
      close (NOTIFFILE_V3);

      unless ($msgV2 eq $msgV3) 
	  {
		error_check("*** ERROR: Different notif messages!\n*** Version 2 notif message:\n\n$msgV2\n\n*** Version 3 notif message:\n\n$msgV3\n", "continue");
	  }
	}
}


sub get_whois3_params
{
  my $version = shift;
  my $config_file;
  my $portset = 0;
  my $hostset = 0;
  
  if ( $version eq "V2" )
  {
    $config_file = $ConfigV2;
  }
  else
  {
    $config_file = $ConfigV3;
  }
	
  open (CONFIG, "$config_file")
    or error_check( "Cannot open config file $config_file ($!)", "fatal");

  while (<CONFIG>)
  {
    if ( /^SVWHOIS_PORT\s+([0-9]+)/ )
	{
	  $port = $1;
	  $portset = 1;
	}

    if ( /^UPDSOURCE\s+\w+\s+(\w+)/ )
	{
	  $host = $1;
	  $hostset = 1;
	}
	
	if ( $portset && $hostset )
	{
	  last;
	}
  }
  
  close(CONFIG);
  
  if ( $portset && $hostset )
  {
    if ( $version eq "V2" )
	{
	  $HostV2 = $host;
	  $PortV2 = $port;
	}
	else
	{
	  $HostV3 = $host;
	  $PortV3 = $port;
	}
	return 1;
  }
  else
  {
    return 0;
  }
}


###################################################################
######
##### Reads the name of an object (primary lookup-key)
#### and passes it to two versions of whois.
### Prints an error if the outputs aren't the same and returns
## the value of the if statement.
#

sub whois_compare
{
    my $lookup = shift;
    my $result1;
    my $result2;
    my $cmd1 = "$PathV2"."utils/whois3/whois3 -h $HostV2 -p $PortV2 $lookup";
    my $cmd2 = "$PathV3"."utils/whois3/whois3 -h $HostV3 -p $PortV3 $lookup";

    $result1 = `$cmd1`;
    $result2 = `$cmd2`;

    # compare the two result strings and report error if they don't match
    if ($result1 ne $result2)
	{
	  error_check( "*** ERROR: Returned whois data don't match!\nWHOIS V2:\n\n$result1\n\n WHOIS V3:\n\n$result2", "continue");
    }
}


sub error_check
{
  my $error_msg = shift;
  my $error_type = shift;
  
  if ( $error_type eq "fatal" )
  {
    print TESTLOG "\nFatal error encountered\n";
	print TESTLOG "\nNumber of updates processed = $Update_count\n";
	print TESTLOG "\nNumber of updates with errors = $Error_count\n";
	print TESTLOG "\n$error_msg\n";
	print TESTLOG "\nTest Program Terminating\n";
	die $error_msg;
  }
  
  if ( $error_type eq "error level reached" )
  {
    print TESTLOG "\nMaximum number of errors encountered\n";
	print TESTLOG "\nNumber of updates processed = $Update_count\n";
	print TESTLOG "\nNumber of updates with errors = $Error_count\n";
	print TESTLOG "\nTest Program Terminating\n";
	die $error_msg;
  }
  
  if ( $error_type eq "end" )
  {
    print TESTLOG "\nTest run completed\n";
	print TESTLOG "\nNumber of updates processed = $Update_count\n";
	print TESTLOG "\nNumber of updates with errors = $Error_count\n";
	print TESTLOG "\nTest Program Terminating\n";
	die $error_msg;
  }
  
  if ( $First_error )
  {
    # this is the first error from the current update.
	# copy the update message to the error log
	
	print TESTLOG "\n-------------------------------------------------------\n";
	
	print TESTLOG "Update causing an error:\n";
	
    open(UPDATE_FILE, "$PathV3"."log/test/"."$Up_file_name") 
      or error_check( "Cannot open update file $PathV3"."log/test/"."$Up_file_name ($!)", "fatal");
      # safe recursive call :)
	
	undef $/ ;   # read whole file in one chunk
	print TESTLOG <UPDATE_FILE>;
	$/ = "\n";
	
	close(UPDATE_FILE);
	
	print TESTLOG "\n\n";
	
	$Error_count++;
	$First_error = 0;
  }
  
  print TESTLOG "\n$error_msg\n";
}
