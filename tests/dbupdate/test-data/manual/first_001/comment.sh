#!/bin/bash
perl -p -i -e's/^UPDSOURCE /###TEST###UPDSOURCE /g' $RIP_CONFIG
