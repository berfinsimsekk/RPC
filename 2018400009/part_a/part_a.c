/*
*@author Berfin Şimşek
*In this code, parent process takes 2 integers input from the user and sends them
*to the child process using pipes in order for child process to execute blackbox executable file
*with the given inputs. Then child process sends the output of the blackbox back to the parent process.
*Parent process writes the result to the file named taken from the arguments. 
*Main idea of the project is creating processes and communicate between them via pipes and executing
*a binary file.
*/


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>




int main(int argc, char const *argv[])
{
	int p2c[2],c2p[2],err[2];  //3 pipes: parent to child, child to parent and last one is for error detection
	pid_t pid; 
	char buffer[100],r_buf[100],e_buf[100]; //3 buffer: to write p2c, to read from c2p, to read from err

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

		int a,b;
		scanf("%d %d",&a,&b);  //Reads the input from the console
		sprintf(buffer,"%d %d\n",a,b); //Since buffer is an char array inputs should converted to string with sprintf
		
		//Sends the inputs that are taken from the console to the child process
		write(p2c[1], buffer, strlen(buffer));

		//Closes the ends of the pipes that are not used in the parent process
		close(p2c[1]);
		close(p2c[0]);    
        close(c2p[1]);
        close(err[1]);

        //Reads the child's output
        int abc = read(c2p[0], r_buf, sizeof(r_buf));
        r_buf[abc]=0;	

        /*
        *Opens a file which has the name taken from the arguments
        *then redirect the stout to this file in order to write the output of the child process
        *to this file
        */ 
        int file = open(argv[2], O_WRONLY | O_CREAT | O_APPEND , 0777); 
		dup2(file,STDOUT_FILENO);
		

		/*
		*Checks whether the error buffer has an element or not. If there is an element, it means
		*blackbox gave an error message and it should print fail. Otherwise, blackbox gave an integer
		*output and it should write success.
		*/
		int num=0;
		num = read(err[0],e_buf,sizeof(e_buf)-1);
		if(num>0){
			printf("FAIL:\n%s",e_buf);
			
		}else{
			printf("SUCCESS:\n%s",r_buf);
			
		}

    	
		//Closes the ends of the pipes that are not used any more
		close(file);
		close(err[0]);

	}else{  

		/*
         * This is the code for the child process.
         */

		//Closes the ends of the pipes that are not used in the child process
		close(p2c[1]); 
		close(err[0]);

		dup2(p2c[0], STDIN_FILENO); //The child associates the read end of forward pipe (p2c) with its own stdin.
		dup2(c2p[1], STDOUT_FILENO); //The child associates the write end of reverse pipe (c2p) with its own stdout.
        dup2(err[1], STDERR_FILENO); //The child associates the write end of pipe that is for error check with stderr.

        execl(argv[1],argv[1],NULL); //blackbox is executed via execl
        
        //Closes the ends of the pipes that are not used any more
        close(c2p[0]);
        close(c2p[1]);
        close(p2c[0]);
        close(err[1]);

	}

	

	return 0;
}