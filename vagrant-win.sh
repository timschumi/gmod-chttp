#!/bin/bash

export DEBIAN_FRONTEND=noninteractive

apt update && apt -y upgrade

apt -y install build-essential cmake mingw-w64
