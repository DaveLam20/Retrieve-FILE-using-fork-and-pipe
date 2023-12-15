#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "retrieve_usp.h"

/*The process and result are not printed in order, if run again at second time, it will be in order*/
int main(){
	int count = 0;
	int i,j;
	char** filenames = NULL;
	
	
	retrieve_files(&filenames,&count);
	for(i = 0; i < count; i++){
		printf("List usp files %s\n", filenames[i]);
	}
	
	int num_pipes = count;
	printf("Number of pipe %d\n", num_pipes);
	int** pipes = (int**)malloc(num_pipes * sizeof(int*));//array for pipe

	for(i=0; i < num_pipes; i++){
		pipes[i] = (int*)malloc(2 * sizeof(int));
		if(pipe(pipes[i])== -1){
			printf("Error Pipe");
			exit(1);
		}	
	}
	
		
	for(i = 0; i < count; i++){	
	pid_t pid = fork();
	
		if (pid < 0){
		printf("Fork-ing error");
	}
	if (pid == 0){	
		printf("Child process:%d - %s\n",getpid(), filenames[i]);
		//Check and close irrelevant pipes
		for(j=0; j < num_pipes; j++){
			if (j != i){
			 close(pipes[j][0]);
			 close(pipes[j][1]);
			}
		}
		write(pipes[i][1], filenames[i],strlen(filenames[i])+1);
		int fd = open(filenames[i], O_RDONLY); //Open usp files
		if (fd == -1){
			printf("Error opening file: %s\n",filenames[i]);
			exit(1);
		}
		//Read the file line by line
		ssize_t readLine(int fd, char* buffer, size_t buffer_size){
		ssize_t bytes_read = 0;
		ssize_t total_bytes = 0;
		char c;
		while(total_bytes < buffer_size - 1){
			bytes_read = read(fd, &c,1);
			if(bytes_read == -1 || bytes_read == 0 || c == '\n'){
			buffer[total_bytes] = '\0';
			return total_bytes;
			}
			else{
				buffer[total_bytes] = c;
				total_bytes++;
			}

		}
		buffer[total_bytes] = '\0';
		return total_bytes;
		}
		//Store the data from usp file for the calculation part
		char id[256];
		readLine(fd, id, sizeof(id));
	

		char num1[256];
		readLine(fd, num1, sizeof(num1));
						
		char operator[256];	
		readLine(fd, operator, sizeof(operator));
		
		
		char num2[256];
		readLine(fd, num2, sizeof(num2));
				
		int result, n1, n2;
		char calculation;

		sscanf(num1, "%d", &n1);
		sscanf(num2, "%d", &n2);
		sscanf(operator,"%s", &calculation);
		switch(calculation){
		case'+':
			result = n2 + n1;
			break;


		case'-':
			result = n1 - n2;
			break;
		case'*':
			result = n1 *  n2;
			break;
		case'/':
			result = n1/ n2;
			break;

		default:
			printf("Invalid operator\n");
			exit(1);

		}
		
		char str[256];
		sprintf(str, "%d", result);	
	//	printf("%s\n",str);

		char result_file[256];
		strcpy(result_file, id);		
		strcat(result_file, "-");
		strcat(result_file, str);
		
		printf("%s\n",result_file);
		free(filenames[i]);
		write(pipes[i][1], &result_file, sizeof(char));
		close(fd);	
		exit(0);	
	}	
}	
		//Parent process
		//When I run the code, it is only print out the name of the file but not the result
		//I could not find out the bug
		int result_fd= open("result.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if(result_fd == -1){
			printf("Error opening result file\n");
			exit(1);
		}

		
		
		for(i = 0; i < count; i++){
		char result_buffer[256];
		ssize_t num_bytes = read(pipes[i][0], result_buffer, sizeof(result_buffer));	
		if(num_bytes > 0){
			write(result_fd, result_buffer, num_bytes);
			write(result_fd,"\n",1);
		}
		close(pipes[i][0]);
		}

		for(i=0; i < num_pipes; i++){
		close(pipes[i][1]);	
		}		
	
		close(result_fd);


	for (i = 0; i < count; i++){
		wait(NULL);
	}
		
	for(i = 0; i < count; i++){
		free(filenames[i]);
	}		
	for(i=0; i < num_pipes; i++){
		free(pipes[i]);
	}	

	free(filenames);
	free(pipes);	
return 0;
}
