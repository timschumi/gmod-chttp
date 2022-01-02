#!/bin/bash -e

CHTTP_DIR=$(realpath $(dirname $0))

# Ensure the machines are up
if ! docker start chttp_x86; then
    docker run --name chttp_x86 -td -v ${CHTTP_DIR}:/vagrant --platform=linux/386 docker.io/debian:8
    docker exec -e PROVISION_NEEDS_STATIC_CMAKE=1 chttp_x86 /vagrant/vagrant-provision.sh
fi

if ! docker start chttp_x64; then
    docker run --name chttp_x64 -td -v ${CHTTP_DIR}:/vagrant --platform=linux/amd64 docker.io/debian:8
    docker exec -e PROVISION_NEEDS_STATIC_CMAKE=1 chttp_x64 /vagrant/vagrant-provision.sh
fi

if ! docker start chttp_win; then
    docker run --name chttp_win -td -v ${CHTTP_DIR}:/vagrant docker.io/debian:10
    docker exec -e PROVISION_NEEDS_MINGW=1 chttp_win /vagrant/vagrant-provision.sh
fi

# Build the different configurations
{
docker exec -u vagrant -w /home/vagrant chttp_x86 /vagrant/build.sh linux Debug $*
docker exec -u vagrant -w /home/vagrant chttp_x86 /vagrant/build.sh linux Release $*
docker exec -u vagrant -w /home/vagrant chttp_x86 /vagrant/build.sh linux Debug static $*
docker exec -u vagrant -w /home/vagrant chttp_x86 /vagrant/build.sh linux Release static $*
} &

{
docker exec -u vagrant -w /home/vagrant chttp_x64 /vagrant/build.sh linux64 Debug $*
docker exec -u vagrant -w /home/vagrant chttp_x64 /vagrant/build.sh linux64 Release $*
docker exec -u vagrant -w /home/vagrant chttp_x64 /vagrant/build.sh linux64 Debug static $*
docker exec -u vagrant -w /home/vagrant chttp_x64 /vagrant/build.sh linux64 Release static $*
} &

{
docker exec -u vagrant -w /home/vagrant chttp_win /vagrant/build.sh win32 Debug $*
docker exec -u vagrant -w /home/vagrant chttp_win /vagrant/build.sh win32 Release $*
docker exec -u vagrant -w /home/vagrant chttp_win /vagrant/build.sh win64 Debug $*
docker exec -u vagrant -w /home/vagrant chttp_win /vagrant/build.sh win64 Release $*
} &

wait

# Copy out artifacts
rm -rf dist
mkdir -p dist

docker cp chttp_x86:/home/vagrant/chttp-linux-Debug/gmsv_chttp_linux.dll dist/gmsv_chttp_linux-dbg.dll
docker cp chttp_x86:/home/vagrant/chttp-linux-Release/gmsv_chttp_linux.dll dist/gmsv_chttp_linux.dll
docker cp chttp_x86:/home/vagrant/chttp-linux-Debug-static/gmsv_chttp_linux.dll dist/gmsv_chttp_linux-dbg-static.dll
docker cp chttp_x86:/home/vagrant/chttp-linux-Release-static/gmsv_chttp_linux.dll dist/gmsv_chttp_linux-static.dll

docker cp chttp_x64:/home/vagrant/chttp-linux64-Debug/gmsv_chttp_linux64.dll dist/gmsv_chttp_linux64-dbg.dll
docker cp chttp_x64:/home/vagrant/chttp-linux64-Release/gmsv_chttp_linux64.dll dist/gmsv_chttp_linux64.dll
docker cp chttp_x64:/home/vagrant/chttp-linux64-Debug-static/gmsv_chttp_linux64.dll dist/gmsv_chttp_linux64-dbg-static.dll
docker cp chttp_x64:/home/vagrant/chttp-linux64-Release-static/gmsv_chttp_linux64.dll dist/gmsv_chttp_linux64-static.dll

docker cp chttp_win:/home/vagrant/chttp-win32-Debug/gmsv_chttp_win32.dll dist/gmsv_chttp_win32-dbg.dll
docker cp chttp_win:/home/vagrant/chttp-win32-Release/gmsv_chttp_win32.dll dist/gmsv_chttp_win32.dll
docker cp chttp_win:/home/vagrant/chttp-win64-Debug/gmsv_chttp_win64.dll dist/gmsv_chttp_win64-dbg.dll
docker cp chttp_win:/home/vagrant/chttp-win64-Release/gmsv_chttp_win64.dll dist/gmsv_chttp_win64.dll
