use 5.006;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

# RIPE NCC common configuration
my %PARAM = (
    'INSTALLDIRS'	=> 'vendor',
    'INSTALLSCRIPT'	=> '$(INSTALLVENDORBIN)', # Hack for Perl prior 5.8.1
    'macro'		=> { 'INSTALLPREFIX' => '$(VENDORPREFIX)' },
    'dist'		=> {
        'COMPRESS'	=> 'gzip', 
            'SUFFIX'	=> '.gz',
            'CI'	=> 'cvs ci',
            'RCS_LABEL'	=> 'cvs tag -c -F $(NAME_SYM)-$(VERSION_SYM)',
    },
    'ABSTRACT'		=> 'RPSL',
    'AUTHOR'		=> 'Kevin Baker <kevin@ripe.net>',
);

WriteMakefile(
    'NAME'		=> 'Net::Whois::RIPE::RPSL::Error',
    'VERSION_FROM'	=> 'Error.pm', # finds $VERSION
    'PREREQ_PM'		=> { }, # e.g., Module::Name => 1.1
    %PARAM
);
