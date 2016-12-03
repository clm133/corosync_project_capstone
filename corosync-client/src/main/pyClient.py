"""
Scirpt for Capstone f2016 - PITT NetApp Team
All rights reserved.

How to run:
	copy this file to the same directory with corosync-client (which is in bin folder).
	
	run with: python pyClient.py
	
	enter 1 to input the same command when running corosync-client individually
	(e.g.  ./corosync-client -a node 11.0.0.1 )
	
	you can check status using option 2 (all nodes) and 3 (single node)
	
"""

import sys
import os
import subprocess
import re
import datetime

# Global variables
#---------------------------------------------
nodes = {} # {ip: time[0.remove_time, 1.add_time 2.remove_flag 3.add_flag 4.recovery_times]}
lastCommand = ''
#---------------------------------------------

# read in and process corosync client command
def processCommand():
	global lastCommand
	
	client_command = raw_input('\nEnter your command now (enter q to quit):\n')
	if (client_command.lower() == 'q'):
		print('\nNow quitting the script...Bye.')
		quit()
	lastCommand = client_command

	p = subprocess.Popen(client_command, stdout = subprocess.PIPE, shell = True)
	(output, err) = p.communicate()
	p_status = p.wait() # status for debug use
	print(output) # print the output(type: str)

	checkAvailability(client_command, output)
	print(' ')

# check if the node is added to the cluster	
def checkAvailability(command, output):
	global nodes
	ipPattern = re.findall(r'[0-9]+(?:\.[0-9]+){3}', command)
	match_ip = ipPattern[0]
	match_add = re.search(r'\bcorosync-client -a\b', command)
	match_remove = re.search(r'\bcorosync-client -r\b', command)
	match_output = re.search(r'\bThere were problems\b', output)
	# change this condition in if statement to match_output for debug use!!!!
	if (not match_output):
		if (match_ip not in nodes):
			if (match_remove):
				times = []
				times.append(datetime.datetime.now())
				times.append('none')
				times.append(1)
				times.append(0)
				times.append(0)
				nodes[match_ip] = times 

			elif (match_add):
				times = []
				times.append('none')
				times.append(datetime.datetime.now())
				times.append(0)
				times.append(1)
				times.append(0)
				nodes[match_ip] = times

		elif (match_ip in nodes):
			temp = nodes.get(match_ip)
			if (match_remove):
				temp[0] = datetime.datetime.now() # store last remove time
				if (temp[2] == 0):
					temp[2] = 1 #set flag
				nodes[match_ip] = temp
			if (match_add):
				temp[1] = datetime.datetime.now() # store last add time
				if (temp[3] == 0): 
					temp[3] = 1 #set flag
				if (temp[2] == 1 and temp[3] == 1): #this means node is recovered
					temp[4] += 1
					temp[2] = 0
					temp[3] = 0
				nodes[match_ip] = temp
				
		else:
			print('Invalid ip address.')


#print the status we want
def printResult(oneOrAll):
	global nodes
	if (oneOrAll == '2'):
		for key in nodes:
			print '\nNode: ', key
			print 'Last remove time: ', nodes.get(key)[0]
			print 'Last recover time: ', nodes.get(key)[1]
			print 'Total recovery times: ', nodes.get(key)[4]
	elif (oneOrAll == '3'):
		k = raw_input('Please enter ip address of the node you want to check:\n')
		if (k in nodes):
			print '\nNode: ', k
			print 'Last remove time: ', nodes.get(k)[0]
			print 'Last recover time: ', nodes.get(k)[1]
			print 'Total recovery times: ', nodes.get(k)[4] 
		else:
			print("invalid ip.")
	else:
		print('Invalid option.')

# main function
def main():
	print('\n---------------Welcome----------------')
	while(1):
		i = raw_input('\n1.Input command\n2.check all nodes\n3.check individual node\n0.quit:\n')
		if (i == '1'): #input command
			processCommand()
		elif (i == '2'): #check status of all nodes
			printResult(i)
		elif (i == '3'): #check status of individual node
			printResult(i)
		elif (i == '0' or i.lower() == 'q'): #quit
			print('\nNow quitting the script...')
			print('Thank you, bye.')
			break
		else:
			print('Unkown input, terminating...')
	quit()

# main script
if __name__ == '__main__':
	main()
