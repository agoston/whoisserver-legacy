#!/bin/bash

TALK=nc -q 1

function issue_command() {
	echo "$@" | $TALK $WHOIS_HOST $SVCONFIG_PORT
}

