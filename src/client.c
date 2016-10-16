#include <stdio.h>
#include <stdlib.h>
#include <corosync/cmap.h>

//boolean stuff
#define true 1
#define false 0
/* our boolean struct (true/false defined above) */
typedef int boolean;

int ui();

int main()
{
	//display ui
	while(1){
		if (ui() == 0){
			exit(0);
		}
	}
}

/*
Text-based UI for the client
*/
int ui()
{
	int choice;
	
	printf("\n");
	printf("-------------------------------------\n");
	printf("           Corosync Client\n");
	printf("-------------------------------------\n");
	printf("Options:\n");
	printf("0. Exit\n");
	printf("-------------------------------------\n");
	printf("\n");
	scanf("%d", &choice);
	if (choice == 0){
		printf("Terminating...\n");
		return 0;
	}
	return choice;
}