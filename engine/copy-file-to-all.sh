#!/bin/bash

# exit when any command fails
set -e

# cd to directory of script
cd "${0%/*}"

# copy file to all pi's
scp $1 lsspi1:~/lss/engine/$1
scp $1 lsspi2:~/lss/engine/$1
scp $1 lsspi3:~/lss/engine/$1
scp $1 lsspi4:~/lss/engine/$1
