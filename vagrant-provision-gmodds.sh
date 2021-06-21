#!/bin/bash -e

mkdir ~/steamcmd
cd ~/steamcmd

curl -sqL "https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz" | tar zxvf -

./steamcmd.sh +force_install_dir ~/gmodds +login anonymous +app_update 4020 validate +quit
