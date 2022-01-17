# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "test-centos7", autostart: false do |test_centos7|
    test_centos7.vm.box = "generic/centos7"

    test_centos7.vm.provision "shell", inline: "yum -y install glibc.i686 libstdc++.i686"
    test_centos7.vm.provision "shell", path: "provision-server.sh", privileged: false
    test_centos7.vm.provision "shell", privileged: false, inline: <<-SHELL
      mkdir -p ~/gmodds/garrysmod/lua/bin ~/gmodds64/garrysmod/lua/bin
      ln -s /vagrant/dist/gmsv_chttp_linux-static.dll ~/gmodds/garrysmod/lua/bin/gmsv_chttp_linux.dll
      ln -s /vagrant/dist/gmsv_chttp_linux64-static.dll ~/gmodds64/garrysmod/lua/bin/gmsv_chttp_linux64.dll
    SHELL
  end

  config.vm.synced_folder ".", "/vagrant", nfs_version: 4
end
