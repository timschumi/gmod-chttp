#!/usr/bin/env bash

set -e

CHTTP_DIR=$(realpath $(dirname $0))

# Ensure the machines are up
{
if ! docker start chttp_x86; then
    docker run --name chttp_x86 -td -v ${CHTTP_DIR}:/vagrant registry.gitlab.steamos.cloud/steamrt/scout/sdk/i386
    docker exec chttp_x86 /vagrant/provision-build.sh
fi
} 2>&1 | sed 's/^/x86 |  /' &

{
if ! docker start chttp_x64; then
    docker run --name chttp_x64 -td -v ${CHTTP_DIR}:/vagrant registry.gitlab.steamos.cloud/steamrt/scout/sdk
    docker exec chttp_x64 /vagrant/provision-build.sh
fi
} 2>&1 | sed 's/^/x64 |  /' &

{
if ! docker start chttp_win; then
    docker run --name chttp_win -td -v ${CHTTP_DIR}:/vagrant docker.io/debian:11
    docker exec -e PROVISION_NEEDS_MINGW=1 chttp_win /vagrant/provision-build.sh
fi
} 2>&1 | sed 's/^/win |  /' &

wait

if [ -z "${CHTTP_VERSION}" ]; then
    CHTTP_VERSION=$(git -C "${CHTTP_DIR}" describe --tags | sed 's/^v//')
fi

# Build the different configurations
{
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x86 /vagrant/build.sh linux Debug $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x86 /vagrant/build.sh linux Release $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x86 /vagrant/build.sh linux Debug static $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x86 /vagrant/build.sh linux Release static $*
} &

{
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x64 /vagrant/build.sh linux64 Debug $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x64 /vagrant/build.sh linux64 Release $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x64 /vagrant/build.sh linux64 Debug static $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_x64 /vagrant/build.sh linux64 Release static $*
} &

{
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_win /vagrant/build.sh win32 Debug $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_win /vagrant/build.sh win32 Release $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_win /vagrant/build.sh win64 Debug $*
docker exec -u vagrant -w /home/vagrant -e CHTTP_VERSION="${CHTTP_VERSION}" chttp_win /vagrant/build.sh win64 Release $*
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
