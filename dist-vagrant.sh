#!/bin/bash -e

# Ensure the machines are up
vagrant up

# Build the different configurations
{
vagrant ssh x86 -c "/vagrant/build.sh linux Debug $*"
vagrant ssh x86 -c "/vagrant/build.sh linux Release $*"
vagrant ssh x86 -c "/vagrant/build.sh linux Debug static $*"
vagrant ssh x86 -c "/vagrant/build.sh linux Release static $*"
} &

{
vagrant ssh x64 -c "/vagrant/build.sh linux64 Debug $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Release $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Debug static $*"
vagrant ssh x64 -c "/vagrant/build.sh linux64 Release static $*"
} &

{
vagrant ssh win -c "/vagrant/build.sh win32 Debug $*"
vagrant ssh win -c "/vagrant/build.sh win32 Release $*"
vagrant ssh win -c "/vagrant/build.sh win64 Debug $*"
vagrant ssh win -c "/vagrant/build.sh win64 Release $*"
} &

wait

# Copy out artifacts
rm -rf dist
mkdir -p dist

vagrant ssh x86 -c "
cp chttp-linux-Debug/gmsv_chttp_linux.dll /vagrant/dist/gmsv_chttp_linux-dbg.dll
cp chttp-linux-Release/gmsv_chttp_linux.dll /vagrant/dist/gmsv_chttp_linux.dll
cp chttp-linux-Debug-static/gmsv_chttp_linux.dll /vagrant/dist/gmsv_chttp_linux-dbg-static.dll
cp chttp-linux-Release-static/gmsv_chttp_linux.dll /vagrant/dist/gmsv_chttp_linux-static.dll
"

vagrant ssh x64 -c "
cp chttp-linux64-Debug/gmsv_chttp_linux64.dll /vagrant/dist/gmsv_chttp_linux64-dbg.dll
cp chttp-linux64-Release/gmsv_chttp_linux64.dll /vagrant/dist/gmsv_chttp_linux64.dll
cp chttp-linux64-Debug-static/gmsv_chttp_linux64.dll /vagrant/dist/gmsv_chttp_linux64-dbg-static.dll
cp chttp-linux64-Release-static/gmsv_chttp_linux64.dll /vagrant/dist/gmsv_chttp_linux64-static.dll
"

vagrant ssh win -c "
cp chttp-win32-Debug/gmsv_chttp_win32.dll /vagrant/dist/gmsv_chttp_win32-dbg.dll
cp chttp-win32-Release/gmsv_chttp_win32.dll /vagrant/dist/gmsv_chttp_win32.dll
cp chttp-win64-Debug/gmsv_chttp_win64.dll /vagrant/dist/gmsv_chttp_win64-dbg.dll
cp chttp-win64-Release/gmsv_chttp_win64.dll /vagrant/dist/gmsv_chttp_win64.dll
"
