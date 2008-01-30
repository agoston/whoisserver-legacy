#!/usr/bin/perl 

### make sure we do things from the current script's directory
BEGIN {
    use FindBin;
    chdir $FindBin::Bin or die "Could not chdir to $FindBin::Bin: $!";
}    

use strict;
use warnings;

use Cwd             qw[cwd];
use File::Spec;
use Getopt::Long;
use File::Basename  qw[dirname basename];

### Set up defaults firsts
my $Cwd         = cwd();
my $DbPrefix    = File::Spec->catdir( $ENV{'HOME'}, 'projects', basename( $Cwd ) );
my $DbPass      = $ENV{'RIPE_DBPASS'} || 'ri:Abpw'; # XXX don't hardcode me?
my $DbPort      = $< . 0;    
my $User        = scalar getpwuid($<);
my $DevelMode   = 0;
my $Verbose     = 0;
my $Debug       = 0;


my $SetupDb         = 0;
my $RunConfigure    = 0;
my $SetupConfig     = 0;
my $Install         = 0;
my $UseTempDir      = 0;
my $SetupTests      = 0;
my $StartDaemon     = 0;
my $All             = 1;

### All options passed;
GetOptions( 
    'verbose!'          => \$Verbose,
    'debug!'            => \$Debug,
    'prefix=s'          => \$DbPrefix,
    'dbpass=s'          => \$DbPass,
    'dbport=s'          => \$DbPort,
    'for-development!'  => \$DevelMode,
    'configure!'        => \$RunConfigure,
    'setup-db!'         => \$SetupDb,
    'setup-config!'     => \$SetupConfig,
    'install!'          => \$Install,
    'use-tempdir!'      => \$UseTempDir,
    'setup-tests!'      => \$SetupTests,
    'start-daemon!'     => \$StartDaemon,
    'all!'              => \$All,
);


### option post-processing
my $DbTempDir;
{   ### we generate a path name for you if you want to use a tempdir -- only used for
    ### for development, not for deployment
    $DbTempDir = File::Spec->catdir( '/tmp', $User, basename( $DbPrefix ) );

    ### set CFLAGS for development mode, if needed
    ### -02 is default, which is production mode
    $ENV{'CFLAGS'} = '-O0 -g' if $DevelMode;
                    
    ### need to make sure java is in our path
    {   my $java = `locate javac | grep bin | head -1 | xargs dirname`;
        chomp $java;
        
        die "Could not find /bin/java in your path -- can not continue\n" unless $java;

        $ENV{'PATH'} .= ':' . $java;
    }
}

if( $RunConfigure or $All ) {
    ### Run autogen first:
    _system( q[sh ./autogen.sh] );
    
    ### configure it to install into $HOME/projects/$name_of_branch
    _system( qq[sh ./configure --with-rdns --prefix=$DbPrefix ].
             qq[--with-cclientinc=$Cwd/third-party/c-client ]. qq[--with-cclientlib=$Cwd/third-party/c-client ] );
}

if( $SetupDb or $All ) {
    ### set up the databases to use. Replaces $USER with your username
    _system( qq[echo "create database ripe_$User"     | mysql -u dbint -p$DbPass] );
    _system( qq[echo "create database ripadmin_$User" | mysql -u dbint -p$DbPass] );
    _system( qq[mysql -u dbint -p$DbPass ripadmin_$User < src/SQL/create.RIPADMIN.sql] ); 
}

if( $SetupConfig or $All ) {
    ### whoisserver comes with a few sample configs
    _system( q[cp conf/rip.config.sample      conf/rip.config]      );
    _system( q[cp conf/sources.config.sample  conf/sources.config]  );  
    _system( q[cp conf/delegations.txt.sample conf/delegations.txt] );
    
    ### the test framework has a config file it uses, which needs to point
    ### at the to-be installed location (same as your --prefix)
    _replace( q[(WHOISDIR\s+=\s+)] => q[${1}${DbPrefix}], 
              q[tests/dbupdate/testing/conf/dataset.config] );
    
    ### change the delcheck configs to point to your installation path
    ### XXX these should probably be replaced with .sample files, and then
    ### copied and THEN edited
    for my $file ( map { chomp; $_ }`ls conf/delcheck-* | grep -v xml` ) {
        _replace( q[/.*/] =>  q[$DbPprefix/conf/], $file );
    }
    
    ### rip.config needs some changes done to it's configuration:
    for my $aref(   
        ### all the port numbers for the server
        [ q[^(SVWHOIS_PORT ).*]     => q[$1] . $DbPort+1 ],
        [ q[^(SVCONFIG_PORT ).*]    => q[$1] . $DbPort+2 ],
        [ q[^(SVMIRROR_PORT ).*]    => q[$1] . $DbPort+3 ],
        [ q[(^UPDSOURCE).*]         => q[$1  DB-TEST localhost,3306,dbint,].
                                       qq[$DbPass,ripe_$User localhost,] .
                                       ($DbPort+1).q[,],($DbPort+4) ],
    
        ### access to the ripadmin db
        [ q[^(RIPADMIN).*]          => q[$1 localhost,3306,dbint,] .
                                       qq[$DbPass,ripadmin_$User]   ], 
        
        ### path to the config file
        [ q[(^SOURCEFILE).*]        => q[$1 $DbPrefix/conf/sources.config] ],
                                     
        ### rdns suffix change
        [ q[^(NS_SUFFIX)\s*ip6.int] => q[$1 e164.arpa] ],
    ) {
    
        my($to,$from) = @$aref;
        _replace( $to => $from, 'conf/rip.config' );
    }
    
    ### sources.config has a few entries that need changing as well
    for my $aref (
        
        ### the database to connect to
        [ q[\nDATABASE\s+.*?/DATABASE] => qq[
#
DATABASE     ripe_$User
host:        localhost
port:        3306
trx_support: y
user:        dbint
password:    $DbPass
/DATABASE
    ] ],
        
        ### comment out the NRTM section, as we dont use NRTM in our tests
        [ q[\nNRTM\s+.*?/NRTM|] => q[
#
#NRTM    SAMPLEM
#host:   master.server.net                                                                         
#port:   4444                                                                                      
#delay:  60                                                                                        
#protocolVersion: 1                                                                                
#/NRTM
    ] ],
        
        [ q[\nSOURCE\s+.*?/SOURCE] => qq[
#
SOURCE    DB-TEST
database: ripe_$User
opMode:   2                                                                                        
updPort:  ] . ($DbPort+4). q[
canupd:   y                                                                                        
deflook:  y                                                                                        
#nrtm:     SAMPLEM                                                                                 
/SOURCE
    ] ],
    
    ) {
        my ($to,$from) = @$aref;
        
        ### multiline replacements, send 's' as a modifier!
        _replace( $to => $from, 'conf/sources.config' => 's' );
    }
}

if( $Install or $All ) {
    ### config set up, run make & install
    _system('make');
    _system('make install');
    
    ### make install doesn't copy all relevant config files :(
    _system( qq[cp -R conf/* $DbPrefix/conf] );

    ### make install also doesn't install all files from bin/SQL
    _system( qq[cp -R src/SQL/* $DbPrefix/bin/SQL] );
}

if( $UseTempDir ) {
    ### OPTIONAL - writes over NFS can be slow/lock, so write logs to local drive ####
    _system( qq[mkdir -p $DbTempDir] ) unless -d $DbTempDir;
    _system( qq[mv $DbPrefix/log $DbTempDir && ln -s $DbTempDir/log $DbPrefix] );
    _system( qq[mv $DbPrefix/var $DbTempDir && ln -s $DbTempDir/var $DbPrefix] );
    _system( qq[mkdir $$DbTempDir/tmp && ln -s $$DbTempDir/tmp tests/dbupdate/testing] );
}

if( $SetupTests or $All ) {
    ### run a single test to set up & populate the db
    ### this test will fail, but the db skeleton will have been created. 
    ### Move on to the nex step
    eval { 
        _system( q[./tests/dbupdate/testing/bin/test.pl -m -d -e -o -u -c ].
                 q[tests/dbupdate/testing/conf/dataset.config -r ].
                 qq[$Cwd/tests/dbupdate/test-data/misc/021] );
    };                 
}

### final steps
{   my $pid_dir = $UseTempDir ? $DbTempDir : $Cwd;
    my $cmd = qq[src/whois-server -p $pid_dir/whoisd.pid -c conf/rip.config];

    print "\nYou can now start the whoisserver using:\n\n\t$cmd\n\n";

    my $test_cmd = q[./tests/dbupdate/testing/bin/test.pl -e -o -u -c ].
                   q[tests/dbupdate/testing/conf/dataset.config -r ].
                   qq[$Cwd/tests/dbupdate/test-data/];

    print "\nYou can then run the test suite using:\n\n\t$test_cmd\n\n";
    print q[
The test program writes it's output to the following directories:
    
    tests/dbupdate/testing/log
    tests/dbupdate/testing/dump

If the whois server console does not show the line 'Mode of operation:' when
running the tests, set it to dynamic mode as follows:

    $ telnet localhost ]. ($DbPort+2) . qq[

And at the prompt type:

    =0= whois R.I.P. config> set dynamic start

    \n\n];

    _system( $cmd ) if $StartDaemon;
}

### run a system call
sub _system {
    my $cmd = shift;
    $cmd .= ' /dev/null 2>&1' unless $Verbose;

    print "Running '$cmd'"  if $Verbose;
    system( $cmd ) and die $?;
    print "....done\n"      if $Verbose;
    
    return 1;
}

### do a search & replace on a file
sub _replace {
    my $from = shift;
    my $to   = shift;
    my $file = shift or die "Need file to operate on";
    my $mods = shift || '';
    
    die "No such file: '$file'" unless -e $file;

    ### make a backup
    _system( qq[cp $file ${file}.bak] );

    my $contents = do { open my $fh, $file or die "Could not open $file: $!"; 
                        local $/; <$fh> 
                    };
    
    my $eval = qq[s|$from|$to|$mods];
    $contents =~ eval $eval or die "Failed to apply '$eval' to $file";
    
    {   open my $fh, ">$file" or die "Could not open $file for writing: $!";
        print $fh, $contents;
        close $fh;
    }        
    
    return 1;
}
    





__END__

### INSTALLATION SECTION ######################################

### Run autogen first:
$ ./autogen.sh

### configure it to install into $HOME/projects/$name_of_branch
$ ./configure --with-rdns --prefix=$DBPREFIX --with-cclientinc=`pwd`/third-party/c-client --with-cclientlib=`pwd`/third-party/c-client

### set up the databases to use. Replaces $USER with your username
$ echo "create database ripe_$USER"     | mysql -u dbint -p$DBPASS
$ echo "create database ripadmin_$USER" | mysql -u dbint -p$DBPASS
$ mysql -u dbint -p$DBPASS ripadmin_$USER < src/SQL/create.RIPADMIN.sql 

### the test framework has a config file it uses, which needs to point
### at the to-be installed location (same as your --prefix)
$ perl -i.bak -pe's|(WHOISDIR\s+=\s+)/.*$|${1}$ENV{DBPREFIX}|' tests/dbupdate/testing/conf/dataset.config

### whoisserver comes with a few sample configs
$ cp conf/rip.config.sample      conf/rip.config
$ cp conf/sources.config.sample  conf/sources.config  
$ cp conf/delegations.txt.sample conf/delegations.txt

### change the delcheck config to point to your installation path
$ perl -i -pe's|/.*/|$ENV{DBPREFIX}/conf/|' `ls conf/delcheck-* | grep -v xml`

### rip.config needs some changes done to it's configuration:
### access to the RIPADMIN DB
$ perl -i.bak -pe's|^(RIPADMIN).*|$1 localhost,3306,dbint,$ENV{DBPASS},ripadmin_$ENV{USER}|' conf/rip.config

$ perl -i.bak -pe's|^(SVWHOIS_PORT ).*|$1.($ENV{DBPORT}+1)|e' conf/rip.config
$ perl -i.bak -pe's|^(SVCONFIG_PORT ).*|$1.($ENV{DBPORT}+2)|e' conf/rip.config
$ perl -i.bak -pe's|^(SVMIRROR_PORT ).*|$1.($ENV{DBPORT}+3)|e' conf/rip.config

### path to the config file
$ perl -i.bak -pe's|(^SOURCEFILE).*|$1 $ENV{DBPREFIX}/conf/sources.config|' conf/rip.config

### rdns suffix change
$ perl -i.bak -pe's|^(NS_SUFFIX)\s*ip6.int|$1 e164.arpa|' conf/rip.config

### access to the update port
$ perl -i.bak -pe's|(^UPDSOURCE).*|"$1 DB-TEST localhost,3306,dbint,$ENV{DBPASS},ripe_$ENV{USER} localhost,".($ENV{DBPORT}+1).q[,].($ENV{DBPORT}+4)|e' conf/rip.config

### fix the entries in sources.config. Finish every command with ^D
### this changes blocks of sample configuration with actual working ones
$ perl -0777 -i.bak -pe's|\nDATABASE\s+.*?/DATABASE|`cat`|se' conf/sources.config <<EOT
#
DATABASE     ripe_$USER
host:        localhost
port:        3306
trx_support: y
user:        dbint
password:    $DBPASS
/DATABASE

### comment out the NRTM section, as we dont use NRTM in our tests
$ perl -0777 -i.bak -pe's|\nNRTM\s+.*?/NRTM|`cat`|se' conf/sources.config <<EOT
#
#NRTM    SAMPLEM
#host:   master.server.net                                                                         
#port:   4444                                                                                      
#delay:  60                                                                                        
#protocolVersion: 1                                                                                
#/NRTM

$ perl -0777 -i.bak -pe's|\nSOURCE\s+.*?/SOURCE|$x=`cat`;$x|se;s|__DBU|$ENV{DBPORT}+4|es' conf/sources.config <<EOT
#
SOURCE    DB-TEST
database: ripe_$USER
opMode:   2                                                                                        
updPort:  __DBU
canupd:   y                                                                                        
deflook:  y                                                                                        
#nrtm:     SAMPLEM                                                                                 
/SOURCE

### config set up, run make & install
$ make
$ make install

### make install doesn't copy all relevant config files :(
$ cp -R conf/* $DBPREFIX/conf

### make install also doesn't install all files from bin/SQL
$ cp -R src/SQL/* $DBPREFIX/bin/SQL

### OPTIONAL - writes over NFS can be slow/lock, so write logs to local drive ####
$ mkdir -p $DBTEMPDIR
$ mv $DBPREFIX/log $DBTEMPDIR && ln -s $DBTEMPDIR/log $DBPREFIX
$ mv $DBPREFIX/var $DBTEMPDIR && ln -s $DBTEMPDIR/var $DBPREFIX
$ mkdir $DBTEMPDIR/tmp && ln -s $DBTEMPDIR/tmp tests/dbupdate/testing


### RUN THE TESTS ################################################################

### run a single test to set up & populate the db
### this test will fail, but the db skeleton will have been created. 
### Move on to the nex step
$ ./tests/dbupdate/testing/bin/test.pl -m -d -e -o -u -c tests/dbupdate/testing/conf/dataset.config -r `pwd`/tests/dbupdate/test-data/misc/021

### start the whoisserver, in a seperate terminal
$ src/whois-server -p $DBTEMPDIR/whoisd.pid -c conf/rip.config

### run the test suite:
./tests/dbupdate/testing/bin/test.pl -e -o -u -c tests/dbupdate/testing/conf/dataset.config -r `pwd`/tests/dbupdate/test-data/

### TROUBLESHOOTING ###############################################################

# if the test hangs, set the server to dynamic mode
# you can tell if the whoisserver doesn't show 'Mode of operation:' when you run the test suite
$ telnet localhost 43002
=0= whois R.I.P. config> set dynamic start

# the interesting log files are written in these directories:
$ ls -l tests/dbupdate/testing/log
$ ls -l tests/dbupdate/testing/dump

