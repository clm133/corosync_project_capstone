#include "sftp_manager.h"

int sftp_conf(ssh_session session, char *source_file,  char *dest_directory)
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
		return CL_SFTP_ERR;
	}
	//initialize sftp session
	rc = sftp_init(sftp);
	if(rc < 0){
		return CL_SFTP_ERR;
	}
	//open destination corosync.conf file 
	dest = sftp_open(sftp, dest_directory, access, S_IRWXU);
	if(dest == NULL){
		sftp_free(sftp);
		return CL_SFTP_ERR;
	}
	//open our source corosync.conf file
	source = fopen(source_file, "r");
	if(source == NULL){
		sftp_close(dest);
		sftp_free(sftp);
		return CL_SFTP_ERR;
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
	return CS_OK;
}

int copy_conf(char *addr, char *source_file,  char *dest_directory)
{
	ssh_session session;
    int rc;
	int port;
    
	//create session
    session = ssh_new();
    if(session == NULL){
		return CL_SFTP_ERR;
	}
    ssh_options_set(session, SSH_OPTIONS_HOST, addr);
	port = 22; //I think this is already default... but just in case
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, "root");
	//connect session
    rc = ssh_connect(session);
    if(rc != SSH_OK){
		free_session(session);
		return CL_SFTP_ERR;
	}
	//authenticate session
    rc = ssh_userauth_password(session, NULL, "root");
    if(rc != SSH_AUTH_SUCCESS){
		free_session(session);
		return CL_SFTP_ERR;
	}
	//sftp corosync.conf file
	rc = sftp_conf(session, source_file, dest_directory);
	if(rc != CS_OK){
    	free_session(session);
		return CL_SFTP_ERR;
	}
	//success - free everything
    free_session(session);
    return CS_OK;
}

int update_all(char **nodelist, int nodelist_size, char *source_file,  char *dest_directory)
{
	int err;
	int flag;
	int i;
	
	flag = CS_OK;
	for(i = 0; i < nodelist_size; i++){
		err = copy_conf(nodelist[i], source_file, dest_directory);
		if(err != CS_OK){
			flag = err;
		}
		err = restart_corosync(nodelist[i]);
		if(err != CS_OK){
			flag = err;
		}
	}
	
	return flag;
}