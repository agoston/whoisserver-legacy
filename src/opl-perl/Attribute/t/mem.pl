#
# This function is used by the test scripts which look for memory 
# leaks.
#
# Arguments: none
# Returns:   memory taken up by the current process
# 

sub mem {
	# sz            SIZE
	# rss           RSS
	# rssize        RSS
	# rsz           RSZ
	# vsize         VSZ
	# vsz           VSZ

	# The SIZE and RSS fields don't count the  page  tables  and
	# the  task_struct of a proc; this is at least 12k of memory
	# that is always resident. SIZE is the virtual size  of  the
	# proc (code+data+stack).

	open(MEM, "ps -o rss -p $$|")
		or die "can't open ps: $!";

	# skip first line (`ps' header)
	<MEM>;
	my $mem = <MEM>;
	$mem+=0;
	return $mem;
	close MEM;
}

1;

