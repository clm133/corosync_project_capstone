# Setting up a 4-Node Server Cluster in VirtualBox

## Installing Virtual Box
* We are using Oracle VirtualBox 5.1.6 to run four virtual servers. 
 
* You can download VirtualBox here: https://www.virtualbox.org/wiki/Downloads
 
1. Run the setup file and it should walk you through a pretty standard installation
2. Congratulations, VirtualBox is installed. When you run VirtualBox it should look something like this:
!(screenshots/virtualbox_installed/virtualbox_installed01)

## Creating a new Virtual Machine
* Ultimately, we want install Ubuntu Server on a VM, but first we have to create a blank VM VirtualBox and alter some settings

1. Open VirtualBox and click the create button. Give the Virtual Machine (VM) a name and in the dropdown menus select Linux and Ubuntu (64-bit). Then click 'next'.
2. VirtualBox will ask you to choose the amount of RAM allocated to the VM. I gave it 1024MB. Then click 'next'.
3. VirtualBox will ask you to add a virtual hard disk. With 'Create a virtual hard disk now' selected click 'create'.
4. VirtualBox will ask you to choose a hard disk file type --choose 'VDI'. 
5. VirtualBox will then ask you how your virtual hard disk should grow --choose 'Dynamically allocated'.
6. VirtualBox will then ask you for the virtual hard disk file location and size --I left the location as default and chose 8GB as my size. Then click 'create'
7. You should return to the main VirtualBox window your newly created VM listed along the lefthand side.
8. Right-click on the VM you just created and select Settings from the dropdown menu that appears.
9. In the settings menu, navigate to the Network options. Under the Adapter 1 tab, change the 'Attached to' selection to 'Bridged Adapter'. 
10. While still in the Network menu, change to the Adapter 2 tab. Check the 'Enable Network Adapter Box', and change the 'Attached to' selection to 'Internal Network'. A text box for naming this network appears, you can name it whatever you want, but make sure that all the other VMs you create use this same name for its internal network.
11. Congratulations, you made your first blank VM! Now lets install Ubuntu on it.

## Installing Ubuntu Server 14.04.5 LTS on a VM
* We are using the VM created above to run Ubuntu Server 14.04.5 LTS 
* Before you start, download the the .iso file here: http://releases.ubuntu.com/14.04/
* Be sure to download the *server* install image and, for the sake of consistency among our setups, download the 64-bit version; however, if you are having problems with the 64-bit version, there might be no issues with the 32-bit version

1. In VirtualBox, with the blank VM selected click 'Start'. You will be prompted to select a virtual optical disk to boot from, choose the Ubuntu .iso file you downloaded earlier.
2. You should soon see the Ubuntu installer screen. It asks for a language first and then takes you to a menu. Choose 'Install Ubuntu Server' at this menu.
3. There will be prompts asking for language, location and asking to detect your keyboard layout (just choose 'no' for this one and manually select your keyboard layout, it's defaulted to United States anyway). 
4. You should finally reach a screen asking to configure the network and choose your primary network during installation. If you set up the networks properly when creating the VM earlier you should see two choices here: eth0 and eth1. Choose eth0 and continue.
5. You will then be asked to provide a host name. You can name this whatever you want, but you should pick something that differentiates it from the 3 other VMs you eventually be running; I used node1, for example.
6. Next you will be asked to provide a name of the user. You can use whatever name you want but I would use the same name across all 4 machines. You will immediately afterwords be asked to provide a username, just use the same name you provided earlier(also use this same username across all 4 VMs).
7. Next you will be asked to provide a password. I kept it simple with 'root', but you can use whatever you want. For your own sanity, use this same password for all 4 VMs.
8. You will then be asked if you want to encrypt your home directory. Just enter 'no'.
9. Ubuntu will ask you if the timezone it thinks you are in is correct. Just enter 'yes'
10. Ubuntu will then ask you for a partitioning method. Just enter 'Guided - use entire disk'. Ubuntu will also ask you to confirm the  partition disk. Confirm it and move on. Then it will ask you if you want to write changes to disk, enter 'yes'.
11. Ubnutu will ask you if you are using a proxy. You probably arn't, so just leave this blank and enter 'continue'.
12. Ubuntu will ask you how to manage updates, choose 'no automatic updates' and continue. Ubuntu will ask if you want to install some commonly used software; we don't need to quite yet, so don't select anything and enter 'continue'.
13. We are almost done! Enter 'yes' when asked to install the GRUB boot loader.
14. Congratulations, Ubuntu is installed on this VM.

## Setting Up Our Internal Network To Use Static IP Addresses
* Before starting to set up our network, we obviously need more than one VM. Repeat the 'Creating a New Virtual Machine' and 'Installing Ubuntu Server 14.04.5 LTS on a VM' 3 more times until you have 4 VMs with Ubuntu installed.
