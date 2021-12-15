/*
*@author Berfin Şimşek
*In this code, inputs are taken from the console and send as parameters to the function which creates a client.
*In this function, client is created and arguments are assigned to the inputs that are taken from the user and server
*is called with these arguments. Also the path of the blackbox file are passed to the server. After server program is completed,
*result is written to a file. 
*Main idea is creating a remote process and executing a binary file.
*/

#include "part_b.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

void
partb_prog_1(char *host, int x, int y, char *blackboxpath, char *pathoutput)
{
	CLIENT *clnt;
	char * *result_1;
	arguments  partb_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, PARTB_PROG, PARTB_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	/*
	*Assigns the arguments to the given parameters and calls the server
	*with these arguments
	*/
	partb_1_arg.a = x;
	partb_1_arg.b = y;
	strcpy(partb_1_arg.path,blackboxpath);
	result_1 = partb_1(&partb_1_arg, clnt);
	
	
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}else{
		
		 /*
        *Opens a file which has the name taken from the arguments
        *then redirect the stout to this file in order to write the output to this file
        */ 
		int file = open(pathoutput, O_WRONLY | O_CREAT | O_APPEND , 0777);
		dup2(file,STDOUT_FILENO);
		printf("%s", *result_1);
		

		close(file);
		
		
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[3];
	char* blackboxpath = argv[1]; //path of the blackbox
	char* pathoutput = argv[2]; //path of the output file
	
	
	int x,y;
	scanf("%d %d",&x,&y); //Takes the inputs from the console
	partb_prog_1 (host,x,y,blackboxpath,pathoutput);



exit (0);
}
