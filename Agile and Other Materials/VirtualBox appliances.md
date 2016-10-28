# Exporting/Importing VirtualBox appliances
## How to
* Under 'File' menu, you can import/export a VirtualBox appliance file which can contain one or many VM images

## Appliance 10/10
### Download here: https://drive.google.com/open?id=0B4lfLgChvxq8OXBBSm1fMW1oNkE
* This contains 5 VMs, 1 client and 4 nodes. 
* Each VM is networked and have ssh enabled using keys, eth0 is Bridged Adapter connection to internet(change as necessary), eth1 is an internal network for client-to-nodes (for ssh and stuff), eth2 is internal network for node-to-node (for corosync stuff)
* The node machines have corosync installed and the client machine has git and gcc installed
* The /etc/corostnc/corosync.conf file on all nodes has had its bindenetaddr variable edited to "10.0.0.0". Change if this is causing problems.

## Appliance 10/28
### Download here: https://drive.google.com/open?id=0B4lfLgChvxq8UlNpT3hQRWxLT2s
* This contains 4 VMS.
* Each VM is networked to internet via NAT eth0. Each VM is networked to each other via internal network eth1.
* Each VM has gcc, make, sshlib, openssh-client, openssh-server, and --of course-- corosync installed.
* root ssh keys have been generated and have been shared among all the nodes.
