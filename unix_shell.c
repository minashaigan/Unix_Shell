#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80 /* The maximum length command */

// a function to read a command line and seperate parameter and store them in args
void getInputs(char input[], char *args[],int *ampersand){

	int i; 		/* loop index */
	int length; /* number of characters in the command line */
	int first;  /* first position of next command parameter */
	int next;   /* index of next parameter into args[] */
	

	length = read(STDIN_FILENO, input, MAX_LINE); /* read what the user enters on the command line */
	first = -1;
	next = 0;	

	//if (length == 0)
    //    exit(0);            /* ^d was entered, end of user command stream */
//    if (length < 0){
//        perror("error reading the command");
//	exit(-1);           /* terminate with error code of -1 */
//    }

	/* examine every character in the input */
	for (i=0;i<length;i++) { 
	    switch (input[i]){
			case ' ':
			case '\t' :               /* argument separators */
				if(first != -1){
			        args[next] = &input[first];    /* set up pointer */
					next++;
				}
			    input[i] = '\0'; /* add a null char; make a C string */
				first = -1;
				break;

	        case '\n':                 /* should be the final char examined */
				if (first != -1){
			        args[next] = &input[first];     
					next++;
				}
			    input[i] = '\0';
			    args[next] = NULL; /* no more arguments to this command */
				break;

			default :             /* some other character */
			if (first == -1)
				first = i;
	        if (input[i] == '&'){
				*ampersand  = 1;
	            input[i] = '\0';
			}
		} 
	}    
	//args[next] = NULL; /* just in case the input line was > 80 */
}


int main(void)
{	   
	char input[MAX_LINE]; /* buffer to hold the command entered */
	char *args[MAX_LINE/2 + 1]; /* command line arguments */
	char *history[MAX_LINE];
	int should_run = 1; /* flag to determine when to exit program */
	int ampersand ; /* waiting for your child or not */
	int command_num = 0;
	while (should_run){
		ampersand = 0;
		printf("osh>");
		fflush(stdout);	
		
		getInputs(input,args,&ampersand); /* get next command */
		
		/* save history */
		char *input_copy = (char *)malloc(MAX_LINE * sizeof(char));
		strcpy(input_copy, args[0]);
		if(command_num<=10 && strcmp(input_copy,"!!") )
			history[command_num] = input_copy;
		else {
			history[command_num] = NULL;
			command_num--;
		}	
		
		if( strcmp(args[0] ,"exit") == 0){
			should_run = 0;	
		}
		else if( strcmp(args[0],"history") == 0){
				int j;
				for(j=1;j<=command_num;j++){ 	
					printf("%d %s\n",command_num-j+1,history[j-1]);	
				}
				continue;
			}

		pid_t pid;

		pid = fork(); /* fork a child process */

		if (pid < 0){ /* error occurred */
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid == 0){ /* child process */	
				if( strcmp(args[0],"!!") == 0){
					if(history[0])
						execvp(history[0],args);
					else
						printf("No commands in history\n");
				}		
				else if(args[0][0]=='!'){
					char a = args[0][1];
					execvp(history[command_num-(a-'0')],args);

					}
		}
		else { /* parent process */
			if(ampersand == 1)
				wait(NULL);
			printf("\n");
		}
		
		/**
		*int j;
		*for(j=0;j<=sizeof(args);j++){
		*	if(!args[j]){
		*		break;
		*	}
		*	printf("args[%d] = \"%s\"\n",j,args[j]);
		*}		
		*/
		
		command_num++;
		/**
		* After reading user input, the steps are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) if command included &, parent will invoke wait()
		*/
	}

	return 0;
}