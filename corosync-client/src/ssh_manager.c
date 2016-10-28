#include "ssh_manager.h"

void free_channel(ssh_channel channel)
{
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);
}

void free_session(ssh_session session)
{
	ssh_disconnect(session);
	ssh_free(session);
}

int start_corosync(char *addr)
{
	ssh_session session;
    ssh_channel channel;
    int rc;
	int port;
	char buffer[1024];
    unsigned int nbytes;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		printf("Could not create ssh session\n");
		return -1;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		printf("Could not connect session\n");
		free_session(session);
		return -1;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		printf("Could not authenticate session\n");
		return -1;
	}
	//create command channel
    channel = ssh_channel_new(session);
    if(channel == NULL){
		free_session(session);
		printf("Could not create channel\n");
	}
	//open channel
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK){
		printf("Could not open channel\n");
		free_channel(channel);
		free_session(session);
	}
	//execute command
    rc = ssh_channel_request_exec(channel, "corosync");
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		printf("Could not execute corosync command on this node\n");
	}
	//read in stuff (if anything)
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        //fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
	//success - free everything
    free_channel(channel);
    free_session(session);
    return 1;
}

int stop_corosync(char *addr)
{
	ssh_session session;
    ssh_channel channel;
    int rc;
	int port;
	char buffer[1024];
    unsigned int nbytes;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		printf("Could not create ssh session\n");
		return -1;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		printf("Could not connect session\n");
		free_session(session);
		return -1;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		printf("Could not authenticate session\n");
		free_session(session);
		return -1;
	}
	//create command channel
    channel = ssh_channel_new(session);
    if(channel == NULL){
		free_session(session);
		printf("Could not create channel\n");
	}
	//open channel
    rc = ssh_channel_open_session(channel);
    if(rc != SSH_OK){
		printf("Could not open channel\n");
		free_channel(channel);
		free_session(session);
	}
	//execute command
    rc = ssh_channel_request_exec(channel, "corosync-cfgtool -H");
    if(rc != SSH_OK){
		free_channel(channel);
		free_session(session);
		printf("Could not execute corosync command on this node\n");
	}
	//read in stuff (if anything)
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        //fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
	//success - free everything
    free_channel(channel);
    free_session(session);
    return 1;
}

int sftp_conf(ssh_session session, char *source_file)
{
	int access;
	FILE *source;
	sftp_session sftp;
	sftp_file dest;
	int rc;
	char buffer[256];
	struct stat st;
	size_t conf_size;
	int buffer_size;
	

	//establish access types
	access = O_WRONLY | O_CREAT | O_TRUNC;
	//get size of source conf file
	stat(source_file, &st);
	conf_size = st.st_size;
	//Create sftp session
	sftp = sftp_new(session);
	if(sftp == NULL){
		printf("Could not start an SFTP session\n");
		return -1;
	}
	//initialize sftp session
	rc = sftp_init(sftp);
	if(rc < 0){
		printf("Could not initialize SFTP session\n");
		return -1;
	}
	//open destination corosync.conf file 
	dest = sftp_open(sftp, "/etc/corosync/corosync.conf", access, S_IRWXU);
	if(dest == NULL){
		printf("Could not open file in remote directory\n");
		sftp_free(sftp);
		return -1;
	}
	//open our source corosync.conf file
	source = fopen(source_file, "r");
	if(source == NULL){
		printf("Could not open file in local directory.\n");
		sftp_close(dest);
		sftp_free(sftp);
		return -1;
	}
	//begin copying
	buffer_size = 256;
	fseek(source, 0, SEEK_SET);
	while(1){
		if(buffer_size > conf_size){
			buffer_size = conf_size;
			fread(&buffer, buffer_size, 1, source);
			sftp_write(dest, buffer, buffer_size);
			break;
		}
		fread(&buffer, buffer_size, 1, source);
		conf_size = conf_size - buffer_size;
		sftp_write(dest, buffer, buffer_size);
	}
	//success
	fclose(source);
	sftp_close(dest);
	sftp_free(sftp);
	return 1;
}

int copy_conf(char *addr, char *source_file)
{
	ssh_session session;
    int rc;
	int port;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		printf("Could not create ssh session\n");
		return -1;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		printf("Could not connect session\n");
		free_session(session);
		return -1;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		printf("Could not authenticate session\n");
		free_session(session);
		return -1;
	}
	//sftp corosync.conf file
	rc = sftp_conf(session, "corosync.conf");
	if(rc == -1){
		printf("something went wrong trying to sftp the conf file\n");
    	free_session(session);
		return -1;
	}
	//success - free everything
    free_session(session);
    return 1;
}