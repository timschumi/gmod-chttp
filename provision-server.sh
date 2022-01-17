#!/bin/bash -e

mkdir ~/steamcmd
cd ~/steamcmd

curl -sqL "https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz" | tar zxvf -

./steamcmd.sh +force_install_dir ~/gmodds +login anonymous +app_update 4020 validate +quit

cp -a ~/gmodds ~/gmodds64

./steamcmd.sh +force_install_dir ~/gmodds64 +login anonymous +app_update 4020 -beta x86-64 validate +quit
