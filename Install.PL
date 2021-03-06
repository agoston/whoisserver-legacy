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
use Data::Dumper;

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
my $UseTempDir      = 1;
my $SetupTests      = 0;
my $StartDaemon     = 0;
my $All             = 1;
my $SkipTest        = 0;

my $DbPrefix    = File::Spec->catdir( $ENV{'HOME'}, 'projects', basename( $Cwd ) );
my $SQLPass     = $ENV{'RIPE_SQLPASS'};
my $SQLUser     = 'dbint';
my $SQLHost     = 'localhost';
my $SQLPort     = 3306;
my $DbPort      = $< . 0;    
my $User        = scalar getpwuid($<);
my $DbName      = "ripe_".$User;
my $DbAdminName = "ripadmin_".$User;
my $RPSLVariant = "RIPE";

### All options passed;
GetOptions( 
    'verbose!'          => \$Verbose,
    'debug!'            => \$Debug,
    'prefix=s'          => \$DbPrefix,
    'sqluser=s'         => \$SQLUser,
    'sqlpass=s'         => \$SQLPass,
    'sqlhost=s'         => \$SQLHost,
    'sqlport=s'         => \$SQLPort,
    'dbport=s'          => \$DbPort,
    # XXX: dbname needs some error handling: not all names are good for the MySQL database.
    'dbname=s'          => sub { $DbName = q{ripe_} . $_[1]; $DbAdminName = q{ripadmin_} . $_[1]; },
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
    'skiptest!'         => \$SkipTest,
) or die usage();

die usage() if $Help;
die 'Need db password. You can also set it via $ENV{RIPE_SQLPASS}'.$/ 
    unless $SQLPass;

print 'Install prefix is '.$DbPrefix."\n";

### Because of our mounting these paths will actually be different texually but
### point to the same inode, make the check a bit more thorough and check the
### actual inode.
my @DbPrefixStat = stat($DbPrefix);
my @CwdStat = stat($Cwd);
die "Source dir and installation dir are identical ($Cwd):\nPlease set --prefix!\n"
    if (@DbPrefixStat && @CwdStat && ($DbPrefixStat[1] eq $CwdStat[1]));

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
  --for-development     Compile the whoisserver with -O0
    
  ### configuration options
  --prefix=%s           Installation prefix to use. Ie, /usr/local/whoisserver. Defaults
                        to \$HOME/projects/`basename pwd`
  --sqluser=%s          Username to use to connect to your MySQL server. Defaults to 'dbint'
  --sqlpass=%s          Password to use to connect to your MySQL server. Defaults to
                        \$ENV{RIPE_SQLPASS}
  --sqlhost=%s          MySQL host to connect to (defaults to 'localhost')
  --sqlport=%d          MySQL port to connect to (defaults to 3306)
  --dbport=%d           Starting number of port range to use for this server instance.
                        Needs 6 consecutive port numbers. Defaults to `id`0.
  --dbname=%s           Database name, just in case you need more than one in parallel.
                        Defaults to your effective username (from /etc/passwd).

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
  --skiptest            Don't run any of the 'make test' commands                          
                        
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
    
    ### die if no sqldb password set
    if (!$SQLPass) {
      die 'Need db password. You can also set it via $ENV{RIPE_SQLPASS}'.$/;
    }
    
    ### 64bit architectures apparently need this or src/opl/librspl.a (and
    ### possibly other libs give this error:
    ### "relocation R_X86_64_32 against `a local symbol' can not be used when 
    ### making a shared object; recompile with -fPIC"
    $ENV{'CFLAGS'} .= ' -fPIC';
    
    ### show the include paths when building
    $ENV{'CFLAGS'} .= ' -v' if $Verbose;

    ### set development/production mode
    if ($DevelMode) {
      $ENV{'CFLAGS'} .= ' -O0 -g';
      $RPSLVariant = "DB-TEST";
    } else {
      $ENV{'CFLAGS'} .= ' -O2 -g';
    }
                    
    ### need to make sure java is in our path
    {   my $java = `locate javac | grep bin | head -1 | xargs dirname`;
        chomp $java;
        
        die "Could not find /bin/javac in your path -- can not continue\n" unless $java;

        $ENV{'PATH'} .= ':' . $java;
    }
}

if( $RunConfigure or $All ) {
    ### Run autogen first:
    _system( q[./autogen.sh] );
    
    ### Then run ./build.sh to build third party extensions
    _system( q[./build.sh] );
    
    ### configure it to install into $HOME/projects/$name_of_branch
    _system( qq[./configure --with-rdns --prefix=$DbPrefix --with-perl=$^X --with-rpslvariant=$RPSLVariant] );

    ### run 'make clean' to start from a new system
    _system( qq[make clean] );
}

if( $SetupDb or $All ) {
    ### set up the databases to use. Replaces $USER with your username
    _system( qq[echo "drop database $DbName"       | mysql -h $SQLHost -P $SQLPort -u $SQLUser -p$SQLPass], 1 );
    _system( qq[echo "create database $DbName"     | mysql -h $SQLHost -P $SQLPort -u $SQLUser -p$SQLPass] );

    _system( qq[echo "drop database $DbAdminName"   | mysql -h $SQLHost -P $SQLPort -u $SQLUser -p$SQLPass], 1 );
    _system( qq[echo "create database $DbAdminName" | mysql -h $SQLHost -P $SQLPort -u $SQLUser -p$SQLPass] );

    _system( qq[cat src/SQL/admin.create.sql src/SQL/admin.data.sql | mysql -h $SQLHost -P $SQLPort -u $SQLUser -p$SQLPass $DbAdminName] );
    _system( qq[cat src/SQL/main.create.sql src/SQL/main.data.sql src/SQL/main.dummy.sql  | mysql -h $SQLHost -P $SQLPort -u $SQLUser -p$SQLPass $DbName] );
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
        [ q[^SVLOOKUP_PORT .*]    => q[SVLOOKUP_PORT ] . ($DbPort+4) ],
        [ q[^UPDSOURCE .*]        => qq[UPDSOURCE DB-TEST $SQLHost,$SQLPort,$SQLUser,$SQLPass,$DbName ].
                                     qq[localhost,].($DbPort+1).q[,].($DbPort+5) ],
    
        ### access to the ripadmin db
        [ q[^RIPADMIN.*]          => qq[RIPADMIN $SQLHost,$SQLPort,$SQLUser,$SQLPass,$DbAdminName] ], 
        
        ### path to the config file
        [ q[^SOURCEFILE .*]       => qq[SOURCEFILE $DbPrefix/conf/sources.config] ],
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
host:        $SQLHost
port:        $SQLPort
trx_support: y
user:        $SQLUser
password:    $SQLPass
/DATABASE
    ] ],
        
        ### comment out the NRTM section, as we dont use NRTM in our tests
        [ q[\nNRTM\s+.*?/NRTM] => q[
#
#NRTM    SAMPLEM
#host:   master.server.net                                                                         
#port:   4444                                                                                      
#delay:  60                                                                                        
#protocolVersion: 2
#/NRTM
    ] ],
        
        [ q[\nSOURCE\s+.*?/SOURCE] => qq[
#
SOURCE    DB-TEST
database: $DbName
opMode:   2                                                                                        
updPort:  ] . ($DbPort+5). q[
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
    # clean dev install dir
    _system(qq[rm -rf $DbPrefix]) unless (! -e $DbPrefix);
    _system(qq[mkdir -p $DbPrefix]);

    ### config set up, run make & install
    ### add gcov flags to the compiler flags for profiling
    ### XXX don't set globally, as the perl bindings below will fail
    ### builing properly with error: RPSL.so: undefined symbol: __gcov_init at 
    ### /usr/lib/perl/5.8/DynaLoader.pm line 225.
    {   my $rc = File::Spec->catfile( $Cwd, '.modulebuildrc' );
        local $ENV{'MODULEBUILDRC'} = "$rc";
        local $ENV{'PERL_MM_OPT'}   = "INSTALL_BASE=$DbPrefix/perl";
        local $ENV{'CFLAGS'}        .= ' -fprofile-arcs -ftest-coverage' if $DevelMode;
        
        ### write an rc file for M::B. No other way, sigh =/ --jib
        _system(qq[echo "install --install_base=$DbPrefix/perl" > $rc] );

        _system('make');
        ### XXX accept failing tests for now, see FB 463
        _system('make test', 1) unless $SkipTest;    

        ### XXX this needs to be 'all-install' in wp1
        ### install the RPSL perl bindings into our project dir
        {   local $ENV{'PERL_MM_OPT'} = "PREFIX=$DbPrefix/perl";
             _system('cd src; make opl-perl-test && make opl-perl-install;');
        }
        _system('make install');
        
        ### throw away the rc file again
        1 while unlink $rc; 
    }
    
    ### make install doesn't copy all relevant config files :(
    _system( qq[cp -R conf/* $DbPrefix/conf] );

    ### make install also doesn't install all files from bin/SQL
    _system( qq[mkdir -p $DbPrefix/bin/SQL] );
    _system( qq[cp -R src/SQL/* $DbPrefix/bin/SQL] );

    {   ### XXX not pretty :(
        my ($lib) = `$^X -MConfig -le'print \$Config{sitelib}'`;
        chomp $lib;
        $lib =~ s/^.*?(perl)/$1/;

        print qq[
You can now use the Net::Whois::RIPE::RPSL, and all the other RIPE::* modules using:

   $^X -I $DbPrefix/$lib -MYour::Module::Here -e'...'

        \n];
    }
}

if( $UseTempDir ) {
    ### OPTIONAL - writes over NFS can be slow/lock, so write logs to local drive ####
    _system(qq[rm -rf $DbTempDir]) unless (! -e $DbTempDir);
    _system(qq[mkdir -p $DbTempDir]);
    _system( qq[mv $DbPrefix/log $DbTempDir && ln -snf $DbTempDir/log $DbPrefix] );
    _system( qq[mv $DbPrefix/var $DbTempDir && ln -snf $DbTempDir/var $DbPrefix] );
    _system( qq[mkdir $DbTempDir/tmp && ln -snf $DbTempDir/tmp tests/dbupdate/testing] );
}

### final steps
{   my $pid_dir = $UseTempDir ? $DbTempDir : $Cwd;
    my $cmd = q[src/whois-server ] . qq[-p $pid_dir/whoisd.pid -c conf/rip.config];

    print '=' x 70;
    print "\n\nYou can now start the whoisserver using:\n\n    \$ $cmd\n\n";

    my $test_cmd = q[$ ./tests/dbupdate/testing/bin/test.pl -e -o -u -c ].
                   q[tests/dbupdate/testing/conf/dataset.config -r ].
                   qq[$Cwd/tests/dbupdate/test-data/];

    print q[
If the whois server console does not show the line 'Mode of operation:' when
running the tests, set it to dynamic mode as follows:

    $ echo set dynamic start | netcat -q1 localhost ] . ($DbPort+2) . $/.$/;

    print "\nYou can then run the test suite using:\n\n     $test_cmd\n\n";
    print q[
The test program writes its output to the following directories:
    
    tests/dbupdate/testing/log
    tests/dbupdate/testing/dump
    ];

    print '=' x 70;
    print $/.$/;

    _system( $cmd ) if $StartDaemon;
}

### run a system call
sub _system {
    my $cmd  = shift;
    my $warn = shift;
    
    $cmd .= ' > /dev/null 2>&1' unless $Debug;

    print "Running '$cmd'\n"  if $Verbose;
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
