#!/usr/bin/perl -w

#####################################################################
###
### Publishes a snapshot of the current whoisserver 'trunk' to the
### public ftp directory. This script is meant to be run nightly from
### cron from dbase@db-int. Check its crontab -l for details.
###
#####################################################################

use strict;

### conf section
my $svn     = 'svn';
my $tar     = 'tar';
my $basedir = '/tmp';
my $uri     = 'file:///ncc/svn/db/whoisserver/trunk'; # XXX use http?
my $name    = 'whoisserver-nightly';
my $outdir  = $basedir . '/' . $name;
my $tarball = $outdir . '.tgz';
my $changes = $outdir . '/ChangeLog';

### gets published automatically max 15 minutes after adding it. See:
### http://iii/twiki/bin/view/OPS/SVCFTPMainAdmin
my $ftpdir  = '/ncc/ftp/ripe/dbase/software';

chdir $basedir or die "Could not chdir to $basedir: $!";

print "Running 'svn export'...";
system("$svn --quiet --force export $uri $outdir") and die "Could not export $uri to $outdir: $?";
print "done\n";

print "Creating logfile...";
system("$svn log $uri > $changes") and die "Could not write $changes: $?";
print "done\n";

print "Creating tarball...";
system("$tar -czf $tarball $name") and die "Could not create $tarball: $?";
print "done\n";

print "Putting tarball in ftp dir...";
system("mv $tarball $ftpdir") and die "Could not copy $tarball to $ftpdir: ?";
print "done\n";

print "Cleaning up tempdir...";
system("rm -rf $outdir") and die "Could not clean up $outdir: $?";
print "done\n";


