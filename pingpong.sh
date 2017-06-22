#!/bin/bash
cd ../solarflare
 
./testperf.sh 2> /dev/null| grep "99.9%=" | sed -e "s/99.9%=//" 