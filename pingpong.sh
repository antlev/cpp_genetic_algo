#!/bin/bash
../solarflare/testperf.sh | grep "99.9%=" | sed -e "s/99.9%=//" 