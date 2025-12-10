#!/usr/bin/env bash

sudo mount -t vboxsf linux_driver ~/linux_driver

# Credit: https://stackoverflow.com/questions/54336626/how-to-create-virtualbox-shared-folder-between-windows-host-and-ubuntu18-04-gues

# After a whole day of trying different solutions I finally found the missing ingredient. But I'm gonna repeat my topic content partially and put here step by step instruction for someone struggling with this task like me.

# We suppose that we already have the Ubuntu 18.04 VM installed and the linux user "myusername" with sudo permissions on this machine.

# So, to share folder D:\test (just as an example) on Windows host machine to Ubuntu 18.04 Virtualbox's guest machine (it's content should be available in /home/myusername/test directory) we make these steps:

# 1) Start Ubuntu VM from VirtualBox interface

# 2) Open "Shared Folder Settings" in the "devices" submenu of the started Ubuntu VM window's upper menu and set the shared folder name as "test" and the path as "/home/myusername/test" and fill the checkbox "Make Permanent"

# 3) Install virtualbox-guest-dkms and virtualbox-guest-utils packages to Ubuntu VM with the package manager.
# sudo apt-get update
# sudo apt-get install virtualbox-guest-dkms
# sudo apt-get install virtualbox-guest-utils

# 4) Create a directory /home/myusername/test on your Ubuntu VM
# mkdir /home/myusername/test

# 5) Restart Ubuntu VM
# reboot

# 6) After rebooting mount shared folder in Ubuntu VM with command
# mount -t vboxsf test /home/myusername/test

# 7) Check files in your /home/myusername/test directory, they should be the same files which are located in your Windows D:\test folder
# ls /home/myusername/test

# 8) Additionally you can make this shared folder auto mounted after rebooting, to do so you need add the next line to your ubuntu user's /home/myusername/.profile file:
# echo "sudo mount -t vboxsf test /home/myusername/test" >> /home/myusername/.profile

# and give your user permission to mount without entering password with opening "sudo visudo" command (which is used to change the /etc/sudoers file) and add the next line:
# myusername ALL = (ALL) NOPASSWD: /bin/mount

# That's it, after these steps I've managed to share files from Windows host to Ubuntu guest machine. I'd like to mention that the shared directory itself and all files inside it belong to root but has 777 rights so there is no problem using them as any user can do whatever he wants with them.

# Share
# Improve this answer
# Follow
# edited Aug 14, 2019 at 18:06
# answered Jan 24, 2019 at 14:06
