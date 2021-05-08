$bashrc_001 = <<-SHELL
# direnv
eval "$(direnv hook bash)"
SHELL

$bashrc_999 = <<-SHELL
# Vagrant work dir.
if [ -d /vagrant ]; then
  cd /vagrant
fi
SHELL

$bashrc= <<-SHELL

#VAGRANT_PROVISION
for SOURCE in ~/.bashrc.d/*;
do
  source $SOURCE
done
SHELL

Vagrant.configure("2") do |config|
  config.vm.box = "generic/debian9"
  config.vm.host_name = "unitctl"
  config.vm.network "private_network", type: "dhcp"
  config.vm.synced_folder ".", "/vagrant"
  config.vm.provision "shell", name: 'packages', inline: <<-SHELL
    curl -sL https://nginx.org/keys/nginx_signing.key | apt-key add -
    printf "deb https://packages.nginx.org/unit/debian/ stretch unit\ndeb-src https://packages.nginx.org/unit/debian/ stretch unit\n" > /etc/apt/sources.list.d/unit.list
    apt update
    apt install -y apt-transport-https ca-certificates
    apt install -y direnv
    apt install -y libcmocka-dev libcmocka0
    apt install -y libcurl4-openssl-dev
    apt install -y unit
    service unit stop # disable unit service!
    apt-get -y install docker-ce docker-ce-cli containerd.io
  SHELL
  config.vm.provision :shell, name: '.bashrc.d', :inline => "echo 'Creating .bashrc.d...' &&  mkdir -p ~/.bashrc.d && chmod 700 ~/.bashrc.d", privileged: false
  config.vm.provision :shell, name: 'direnv.bashrc', :inline => "echo -e '#{$bashrc_001}' > ~/.bashrc.d/001-direnv && chmod +x ~/.bashrc.d/001-direnv", privileged: false
  config.vm.provision :shell, name: 'vagrant.bashrc', :inline => "echo -e '#{$bashrc_999}' > ~/.bashrc.d/999-vagrant && chmod +x ~/.bashrc.d/999-vagrant", privileged: false
  config.vm.provision :shell, name: '.bashrc', :inline => "DONE=`grep -Fq \\#VAGRANT_PROVISION ~/.bashrc /dev/null; echo $?` && if [ $DONE -eq 1 ]; then echo '#{$bashrc}' >> ~/.bashrc; else echo '.bashrc already patched.'; fi", privileged: false
  config.vm.provider :vmware_desktop do |vw|
    vw.gui = true
    #vw.memory = 2048
    #vw.cpus = 2
    end
end
