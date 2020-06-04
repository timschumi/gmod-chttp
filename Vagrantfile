# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "x64" do |x64|
    x64.vm.box = "generic/debian9"
  end

  config.vm.define "x86" do |x86|
    x86.vm.box = "generic-x32/debian9"
  end

  config.vm.synced_folder ".", "/vagrant", nfs_version: 4

  config.vm.provision "shell", inline: <<-SHELL
    echo "deb http://ftp.us.debian.org/debian/ stretch-backports main" >> /etc/apt/sources.list
    echo "deb http://ftp.us.debian.org/debian/ stretch-backports-sloppy main" >> /etc/apt/sources.list

    export DEBIAN_FRONTEND=noninteractive

    apt update && apt -y upgrade

    apt -y install build-essential
    apt -y -t stretch-backports-sloppy install libarchive13
    apt -y -t stretch-backports install cmake
  SHELL
end
