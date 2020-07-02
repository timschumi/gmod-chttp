#!/bin/bash

echo "deb http://ftp.us.debian.org/debian/ stretch-backports main" >> /etc/apt/sources.list
echo "deb http://ftp.us.debian.org/debian/ stretch-backports-sloppy main" >> /etc/apt/sources.list

export DEBIAN_FRONTEND=noninteractive

apt update && apt -y upgrade

apt -y install build-essential
apt -y -t stretch-backports-sloppy install libarchive13
apt -y -t stretch-backports install cmake
