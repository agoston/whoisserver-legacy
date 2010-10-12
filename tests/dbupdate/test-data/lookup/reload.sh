#!/bin/bash

TALK="nc -q 1"

function issue_command() {
	echo "$@" | $TALK $WHOIS_HOST $SVCONFIG_PORT >/dev/null
}

# output newline for better output when debugging
echo

for i in $*
do
    issue_command set initrx $i
done
