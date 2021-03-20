#!/bin/bash -e

export DEBIAN_FRONTEND=noninteractive

apt update && apt -y upgrade

apt -y install build-essential

if [ -n "$PROVISION_NEEDS_STATIC_CMAKE" ]; then
    # Make sure to remove the preinstalled CMake
    apt -y purge --auto-remove cmake

    CMAKE_RELEASE="3.19.7_1"
    if [ "$(uname -m)" = "x86_64" ]; then
        CMAKE_ARCHIVE="cmake-linux-x64.tar.gz"
    else
        CMAKE_ARCHIVE="cmake-linux-x86.tar.gz"
    fi

    wget -q "https://github.com/timschumi/cmake-static/releases/download/${CMAKE_RELEASE}/${CMAKE_ARCHIVE}"
    tar -xf "${CMAKE_ARCHIVE}" -C /usr/local
    rm -rf "${CMAKE_ARCHIVE}"
fi

if [ -n "$PROVISION_NEEDS_MINGW" ]; then
	apt -y install mingw-w64
fi
