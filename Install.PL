#!/usr/bin/perl 


### make sure we do things from the current script's directory
BEGIN {
    use FindBin;
    chdir $FindBin::Bin or die "Could not chdir to $FindBin::Bin: $!";
}    

use strict;
use warnings;

use Cwd             qw[cwd];
use Config;
use File::Spec;
use Getopt::Long;
use File::Basename  qw[dirname basename];

### Set up defaults firsts
my $Cwd         = cwd();

my $DevelMode   = 0;
my $Verbose     = 0;
my $Debug       = 0;
my $Help        = 0;

my $SetupDb         = 0;
my $RunConfigure    = 0;
my $SetupConfig     = 0;
my $Install         = 0;
my $UseTempDir      = 0;
my $SetupTests      = 0;
my $StartDaemon     = 0;
my $All             = 1;

my $DbPrefix    = File::Spec->catdir( $ENV{'HOME'}, 'projects', basename( $Cwd ) );
my $DbPass      = $ENV{'RIPE_DBPASS'} || 'ri:Abpw'; # XXX don't hardcode me?
my $DbUser      = 'dbint';
my $DbPort      = $< . 0;    
my $User        = scalar getpwuid($<);
my $DbName      = "ripe_".$DbUser;
my $DbAdminName = "ripadmin_".$DbUser;

### All options passed;
GetOptions( 
    'verbose!'          => \$Verbose,
    'debug!'            => \$Debug,
    'prefix=s'          => \$DbPrefix,
    'dbuser=s'          => \$DbUser,
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
    'help!'             => \$Help,
) or die usage();

die usage() if $Help;

### documented here so it's close to the original options
sub usage {
    my $me = basename($0);

    return qq[
Usage: $0 --verbose --all --start-daemon    

Sets up and installs a whoisserver from a clean checkout or tarball.
Most stages can be individually run. See the options below for details.

Options:

  ### flags
  --help                Print this message
  --verbose             Print diagnostic messages (recommended)
  --debug               Print output from all childprocesses (recommended for developers)
  --for-development     Compile the whoisserver with debug symbols.
    
  ### configuration options
  --prefix=%s           Installation prefix to use. Ie, /usr/local/whoisserver. Defaults
                        to \$HOME/projects/`basename pwd`
  --dbuser=%s           Username to use to connect to your MySQL server. Defaults to 'dbint'
  --dbpass=%s           Password to use to connect to your MySQL server. Defaults to
                        \$ENV{RIPE_DBPASS}
  --dbport=%d           Starting number of port range to use for this server instance.
                        Needs 5 consecutive port numbers. Defaults to `id`0.

  ### build/configure/install options
  --all                 Do all of the below operations. Can be negated by --no-all.
                        Defaults to true.
  --configure           Run ./autogen.sh and ./configure with the right arguments.
                        --all implies --configure
  --setup-db            Set up the MySQL databases needed by the whoisserver. This
                        defaults to ripe_\$UserName and ripadmin_\$UserName. 
                        --all implies --setup-db
  --setup-config        Munge the configuration files used by the whoisserver and
                        its test suite: conf/rip.config, conf/sources.config,
                        tests/dbupdate/testing/conf/dataset.config and conf/delcheck-*
                        --all implies --setup-config
  --install             Runs make, make install and copies the config and sql files
                        to your installation directory. --all implies --install
  --setup-tests         Run a sample test case to setup the skeleton database.
                        --all implies --setup-tests
                        
  ### additional actions
  --use-tempdir         All temp, var and log dirs are written under /tmp for
                        added performance. This is recommended if your main
                        files are on a network volume.
  --start-daemon        At the end of the process, start the whoisserver
        
Examples:

  ### setup a whoisserver from a clean checkout, suited for developers:
  \$ $^X $me --verbose --debug --for-development --use-tempdir --all --start-daemon

  ### just (re)start the whoisserver
  \$ $^X $me --no-all --start-daemon

    \n];
}

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
    _system( qq[echo "drop database ripe_$User"       | mysql -u $DbUser -p$DbPass], 1 );
    _system( qq[echo "create database ripe_$User"     | mysql -u $DbUser -p$DbPass] );

    _system( qq[echo "drop database ripadmin_$User"   | mysql -u $DbUser -p$DbPass], 1 );
    _system( qq[echo "create database ripadmin_$User" | mysql -u $DbUser -p$DbPass] );

    ### XXX can't we setup ripe db like this?
    _system( qq[mysql -u $DbUser -p$DbPass ripadmin_$User < src/SQL/create.RIPADMIN.sql] ); 
}

if( $SetupConfig or $All ) {
    ### whoisserver comes with a few sample configs
    _system( q[cp conf/rip.config.sample      conf/rip.config]      );
    _system( q[cp conf/sources.config.sample  conf/sources.config]  );  
    _system( q[cp conf/delegations.txt.sample conf/delegations.txt] );
    
    ### the test framework has a config file it uses, which needs to point
    ### at the to-be installed location (same as your --prefix)
    ### This file is generated, so needs no .sample entry
    _replace( q[^WHOISDIR\s+=\s+.*$] => qq[WHOISDIR = $DbPrefix], 
              q[tests/dbupdate/testing/conf/dataset.config] => 'm' );
    
    ### change the delcheck configs to point to your installation path
    ### copy the sample to the real file, then edit it
    for my $file ( map { chomp; $_ } `ls conf/delcheck-*.sample | grep -v xml` ) {
        my $new = $file;
        $new =~ s/\.sample$//;
       
        _system( qq[cp $file $new] );
        _replace( q[/.*/] =>  qq[$DbPrefix/conf/], $new );
    }
    
    ### rip.config needs some changes done to it's configuration:
    for my $aref(   
        ### all the port numbers for the server
        [ q[^SVWHOIS_PORT .*]     => q[SVWHOIS_PORT ]  . ($DbPort+1) ],
        [ q[^SVCONFIG_PORT .*]    => q[SVCONFIG_PORT ] . ($DbPort+2) ],
        [ q[^SVMIRROR_PORT .*]    => q[SVMIRROR_PORT ] . ($DbPort+3) ],
        [ q[^UPDSOURCE .*]        => q[UPDSOURCE DB-TEST localhost,3306,$DbUser,].
                                     qq[$DbPass,ripe_$User localhost,] .
                                     ($DbPort+1).q[,].($DbPort+4) ],
    
        ### access to the ripadmin db
        [ q[^RIPADMIN.*]          => q[RIPADMIN localhost,3306,$DbUser,] .
                                     qq[$DbPass,ripadmin_$User]   ], 
        
        ### path to the config file
        [ q[^SOURCEFILE .*]       => q[SOURCEFILE $DbPrefix/conf/sources.config] ],
                                     
        ### rdns suffix change
        [ q[^NS_SUFFIX\s*ip6.int] => q[NS_SUFFIX e164.arpa] ],
    ) {

        my($to,$from) = @$aref;
        _replace( $to => $from, 'conf/rip.config' => 'm' );
    }
    
    ### sources.config has a few entries that need changing as well
    for my $aref (
        
        ### the database to connect to
        [ q[\nDATABASE\s+.*?/DATABASE] => qq[
#
DATABASE     $DbName
host:        localhost
port:        3306
trx_support: y
user:        $DbUser
password:    $DbPass
/DATABASE
    ] ],
        
        ### comment out the NRTM section, as we dont use NRTM in our tests
        [ q[\nNRTM\s+.*?/NRTM] => q[
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
database: $DbName
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
    ### add gcov flags to the compiler flags for profiling
    ### XXX don't set globally, as the perl bindings below will fail
    ### builing properly with error: RPSL.so: undefined symbol: __gcov_init at 
    ### /usr/lib/perl/5.8/DynaLoader.pm line 225.
    {   local $ENV{'CFLAGS'} .= ' -fprofile-arcs -ftest-coverage' if $DevelMode;

        _system('make');
        _system('make install');
    }
    
    ### make install doesn't copy all relevant config files :(
    _system( qq[cp -R conf/* $DbPrefix/conf] );

    ### make install also doesn't install all files from bin/SQL
    _system( qq[cp -R src/SQL/* $DbPrefix/bin/SQL] );

    ### install the RPSL perl bindings into our project dir
    {   local $ENV{'PERL_MM_OPT'} = "PREFIX=$DbPrefix/perl";
        _system('cd src; make opl-perl-test && make opl-perl-install;');
    }

    print qq[
You can now use the Net::Whois::RIPE::RPSL module using:

   $^X -I $DbPrefix/perl/lib/perl/$Config{version} -MNet::Whois::RIPE::RPSL -e'...'

   \n];

}

if( $UseTempDir ) {
    ### OPTIONAL - writes over NFS can be slow/lock, so write logs to local drive ####
    _system( qq[mkdir -p $DbTempDir] ) unless -d $DbTempDir;
    _system( qq[mv $DbPrefix/log $DbTempDir && ln -s $DbTempDir/log $DbPrefix] );
    _system( qq[mv $DbPrefix/var $DbTempDir && ln -s $DbTempDir/var $DbPrefix] );
    _system( qq[mkdir $DbTempDir/tmp && ln -s $DbTempDir/tmp tests/dbupdate/testing] );
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


    print '=' x 70;
    print "\n\nYou can now start the whoisserver using:\n\n    $cmd\n\n";

    my $test_cmd = q[./tests/dbupdate/testing/bin/test.pl -e -o -u -c ].
                   q[tests/dbupdate/testing/conf/dataset.config -r ].
                   qq[$Cwd/tests/dbupdate/test-data/];

    print "\nYou can then run the test suite using:\n\n    $test_cmd\n\n";
    print q[
The test program writes its output to the following directories:
    
    tests/dbupdate/testing/log
    tests/dbupdate/testing/dump

If the whois server console does not show the line 'Mode of operation:' when
running the tests, set it to dynamic mode as follows:

    $ telnet localhost ]. ($DbPort+2) . qq[

And at the prompt type:

    =0= whois R.I.P. config> set dynamic start

    \n\n];

    print '=' x 70;
    print $/.$/;



    _system( $cmd ) if $StartDaemon;
}

### run a system call
sub _system {
    my $cmd  = shift;
    my $warn = shift;
    
    $cmd .= ' > /dev/null 2>&1' unless $Debug;

    print "Running '$cmd'"  if $Verbose;
    system( $cmd ) and do { $warn ? warn $? : die $? };
    print "....done\n"      if $Verbose;
    
    return 1;
}

### do a search & replace on a file
### XXX using $1 and friends in the replacement section doesn't currently work
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

    print "Replacing $from in $file";
    my $re = qq[s|(?$mods:$from)|$to|];
    
    eval qq[\$contents =~ $re] or die qq[Failed to apply $re to $file];
    
    {   open my $fh, ">$file" or die "Could not open $file for writing: $!";
        print $fh $contents;
        close $fh;
    }        
    print "....done\n";
    
    
    return 1;
}
    

