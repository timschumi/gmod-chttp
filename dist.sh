#!/bin/bash -e

# Ensure the machines are up
vagrant up

# Empty dist folder
rm -rf dist

# Build the different configurations
vagrant ssh x86 -c "/vagrant/build.sh linux Debug $*"
vagrant ssh x86 -c "/vagrant/build.sh linux Release $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Debug $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Release $*"
vagrant ssh x86 -c "/vagrant/build.sh linux Debug static $*"
vagrant ssh x86 -c "/vagrant/build.sh linux Release static $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Debug static $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Release static $*"
vagrant ssh win -c "/vagrant/build.sh win32 Debug $*"
vagrant ssh win -c "/vagrant/build.sh win32 Release $*"
vagrant ssh win -c "/vagrant/build.sh win64 Debug $*"
vagrant ssh win -c "/vagrant/build.sh win64 Release $*"
