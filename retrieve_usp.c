#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void retrieve_files(char*** filenames, int *count){
	
	DIR* dir;
	struct dirent* ent;
	
	//Open the current director
	dir = opendir(".");
	ent = readdir(dir);

	if (dir == NULL){
		perror("Unable to search directory");
	}

		while (ent != NULL){
		if (strlen(ent->d_name) > 4 && strcmp(ent->d_name + strlen(ent->d_name)-4, ".usp")==0){
			(*filenames) = (char**)realloc((*filenames),((*count)+1)*sizeof(int*));
			(*filenames)[*count] = (char*)malloc((strlen(ent->d_name)+1)*sizeof(char));
			strcpy((*filenames)[*count],ent->d_name);
			(*count)++;
			
			}
		
	ent= readdir(dir);
	}
	closedir(dir);
	

}

