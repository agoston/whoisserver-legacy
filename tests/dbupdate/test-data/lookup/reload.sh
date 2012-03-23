#!/bin/bash

function issue_command() {
	echo "$@" | nc $WHOIS_HOST $SVCONFIG_PORT >/dev/null
}

# output newline for better output when debugging
echo

for i in $*
do
    issue_command set initrx $i
done
