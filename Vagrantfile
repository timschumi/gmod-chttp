# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "x64" do |x64|
    x64.vm.box = "generic/debian9"
    x64.vm.provision "shell", path: "vagrant-deb9.sh"
  end

  config.vm.define "x86" do |x86|
    x86.vm.box = "generic-x32/debian9"
    x86.vm.provision "shell", path: "vagrant-deb9.sh"
  end

  config.vm.define "win" do |win|
    win.vm.box = "generic/debian10"
    win.vm.provision "shell", path: "vagrant-win.sh"
  end

  config.vm.synced_folder ".", "/vagrant", nfs_version: 4
end
