#!/bin/bash -e

# Ensure the machines are up
vagrant up

# Empty dist folder
rm -rf dist

# Build the different configurations
vagrant ssh x86 -c "/vagrant/dist-build-single.sh linux Debug"
vagrant ssh x64 -c "/vagrant/dist-build-single.sh linux64 Debug"
vagrant ssh x86 -c "/vagrant/dist-build-single.sh linux Release"
vagrant ssh x64 -c "/vagrant/dist-build-single.sh linux64 Release"
vagrant ssh win -c "/vagrant/dist-build-single.sh win32 Debug"
vagrant ssh win -c "/vagrant/dist-build-single.sh win64 Debug"
vagrant ssh win -c "/vagrant/dist-build-single.sh win32 Release"
vagrant ssh win -c "/vagrant/dist-build-single.sh win64 Release"
