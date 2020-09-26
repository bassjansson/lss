#!/bin/bash

# exit when any command fails
set -e

# cd to directory of script
cd "${0%/*}"

# copy rc.local
sudo cp ./rc.local /etc/
