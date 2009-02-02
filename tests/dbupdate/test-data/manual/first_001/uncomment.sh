#!/bin/bash
perl -p -i -e's/^###TEST###UPDSOURCE /UPDSOURCE /g' $RIP_CONFIG
