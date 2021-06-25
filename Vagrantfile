# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "x64" do |x64|
    x64.vm.box = "generic/debian8"
    x64.vm.provision "shell", path: "vagrant-provision.sh", env: {
      "PROVISION_NEEDS_STATIC_CMAKE" => "1",
    }
  end

  config.vm.define "x86" do |x86|
    x86.vm.box = "timschumi/debian8-x32"
    x86.vm.provision "shell", path: "vagrant-provision.sh", env: {
      "PROVISION_NEEDS_STATIC_CMAKE" => "1",
    }
  end

  config.vm.define "win" do |win|
    win.vm.box = "generic/debian10"
    win.vm.provision "shell", path: "vagrant-provision.sh", env: {
      "PROVISION_NEEDS_MINGW" => "1",
    }
  end

  config.vm.define "test-centos7", autostart: false do |test_centos7|
    test_centos7.vm.box = "generic/centos7"

    test_centos7.vm.provision "shell", inline: "yum -y install glibc.i686 libstdc++.i686"
    test_centos7.vm.provision "shell", path: "vagrant-provision-gmodds.sh", privileged: false
    test_centos7.vm.provision "shell", privileged: false, inline: <<-SHELL
      mkdir -p ~/gmodds/garrysmod/lua/bin ~/gmodds64/garrysmod/lua/bin
      ln -s /vagrant/dist/gmsv_chttp_linux-static.dll ~/gmodds/garrysmod/lua/bin/gmsv_chttp_linux.dll
      ln -s /vagrant/dist/gmsv_chttp_linux64-static.dll ~/gmodds64/garrysmod/lua/bin/gmsv_chttp_linux64.dll
    SHELL
  end

  config.vm.synced_folder ".", "/vagrant", nfs_version: 4
end
