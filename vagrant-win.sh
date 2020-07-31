#!/bin/bash -e

export DEBIAN_FRONTEND=noninteractive

# HACK: Manually tell debian where to install grub
debconf-set-selections <<< "grub-pc grub-pc/install_devices string /dev/vda"

apt update && apt -y upgrade

apt -y install build-essential cmake mingw-w64
