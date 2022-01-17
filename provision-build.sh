#!/bin/bash -e

export DEBIAN_FRONTEND=noninteractive

apt-get update && apt-get -y upgrade

apt-get -y install build-essential cmake

if [ -n "$PROVISION_NEEDS_MINGW" ]; then
    apt-get -y install mingw-w64
fi

# Create vagrant user if necessary
if ! id -u vagrant 2> /dev/null; then
    mkdir -p /home
    useradd -s /bin/bash -m vagrant
fi
