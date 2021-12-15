/*
*@author Berfin Şimşek
*In this code, there is a server program that creates a child process that executes the given blackbox and return
*the response to the client.
*Main idea is using RPC as the layer of communication between client and server.
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

char **
partb_1_svc(arguments *argp, struct svc_req *rqstp)
{
	static char * result;

	int p2c[2],c2p[2],err[2]; //3 pipes: parent to child, child to parent and last one is for error detection
	pid_t pid;
	char buffer[10000],r_buf[10000],e_buf[10000]; //3 buffer: to write p2c, to read from c2p, to read from err

	// Pipes are created
	pipe(p2c);
	pipe(c2p);
	pipe(err);

	if( (pid=fork()) == -1 ){
		fprintf(stderr, "fork() failed. \n");
		exit(-1);

	}

	if (pid > 0){ 
		 /*
         * This is the code for the parent process.
         */

		sprintf(buffer,"%d %d\n",argp->a,argp->b); //Arguments of the client are converted to string
		write(p2c[1], buffer, strlen(buffer)); //Sends the inputs that are taken from the client to the child process

		//Closes the ends of the pipes that are not used in the parent process
		close(p2c[1]);
		close(p2c[0]);    
        close(c2p[1]);
        close(err[1]);

        //Reads the child's output
        int abc = read(c2p[0], r_buf, sizeof(r_buf));
        r_buf[abc]=0;

        int num = 0;
        char fail[]="FAIL:\n";
        char success[]="SUCCESS:\n";

        /*
		*Checks whether the error buffer has an element or not. If there is an element, it means
		*blackbox gave an error message and it should print fail. Otherwise, blackbox gave an integer
		*output and it should write success.
		*/
        num = read(err[0],e_buf,sizeof(e_buf)-1);
        if(num>0){
        	strcat(fail,e_buf); //Concatenates the fail string with the error message
        	result = fail;
		}else{
			strcat(success,r_buf); //Concatenates the success string with the output
			result = success;
		}

      
      	return &result;

	}else{  

		/*
         * This is the code for the child process.
         */

		//Closes the ends of the pipes that are not used in the child process
		close(p2c[1]); 
		close(err[0]);

		dup2(p2c[0], STDIN_FILENO); //The child associates the read end of forward pipe (p2c) with its own stdin.
		dup2(c2p[1], STDOUT_FILENO); //The child associates the write end of reverse pipe (c2p) with its own stdout and stderr.
        dup2(err[1], STDERR_FILENO); //The child associates the write end of pipe that is for error check with stderr.

        execl(argp->path,argp->path,NULL); //blackbox is executed via execl
        
         //Closes the ends of the pipes that are not used any more
        close(c2p[0]);
        close(c2p[1]);
        close(p2c[0]);
        close(err[1]);
	}



	
}
