
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "headers/stringfunc.h"
#include "headers/globals.h"
#include "headers/headerhandlers.h"

#define TRUE 1
#define FALSE 0

/*Function Protoype Definitions*/
int getproperfilefordir(char*);

void setfilesize(char*);

int getfd(char*,int);

int isdirectory(char*,char*);

void setfilesize(char*);

char* getmimetype(char*);

/*
 * Input -> resource name and mode [error mode and normal mode] 
 * Output -> file discripter if successfull else -1 
 * Description -> check if resource exsits within the file system and return 
 * 		the proper fd.
 */
int getfd(char *resource,int mode)
{
	char *path, *filepath_1, *filepath_2, *errorpath;
	int filediscripter, ret;

	path = (char*)malloc(1024);
	
	switch(mode)
	{

		case 1: /* Normal Mode*/
			filepath_1 = "servercontents/";
			filepath_2 = "servercontents";
	
			if(resource[0]!='/') /*if the request does not start with a '/'*/
			{
				ret = sprintf(path,"%s%s",filepath_1,resource);
				if(isdirectory(path,resource)) /*check if directory*/ 
				{
					return getproperfilefordir(path);  
				}
					
			}
			else
			{
				if(resource[1]!='\0') 
				{
					ret = sprintf(path,"%s%s",filepath_2,resource);
					if(isdirectory(path,resource)) /*check if directory*/
					{	
						return getproperfilefordir(path);  
					}	
				}

				else /*if the requested resource is only a '/'*/
				{
					ret = sprintf(path,"%s%s",filepath_2,resource);
					return getproperfilefordir(path);	
				}
			}		
			
			break;

		case 2: /*Error Mode*/
			errorpath = "servercontents/errors/";
			ret = sprintf(path,"%s%s",errorpath,resource);
			break;
	}

	/*if the requested resource is a file return file discripter*/		
	if((filediscripter=open(path,O_RDONLY))!=-1)
	{
		setfilesize(path);
		finalpath = path;
		return filediscripter;
	}

	return -1;
}

/*
 * Input -> path specified
 * Ouput -> 1 if it is a directory else 0 if
 * Discription -> function to check whether a specified path is a directory
 */
int isdirectory(char* pathin,char* resource)
{
	DIR *dir;
	int length_path = 0, length_resource = 0;
	int size = 0;
	
	char *header, *path;	

	if((dir = opendir(pathin))==NULL)
	{
		return FALSE;
	}
	
	length_path = strlen(pathin);
	length_resource = strlen(resource);

	/*if a directory is requested and there is no '/' at the end send the browser
  	 * move permanetly header asking to add a / at the end 	
	 */	
	if(pathin[length_path-1]!='/')
	{
		path = (char*)malloc(length_resource+1);
		strcpy(path,resource);
		strcat(path,"/");
		header = getspecialheader(path);
		size = strlen(header);
		send(new_fd,header,size,0);	
	}
	return TRUE;
}

/*
 * Input -> path to file
 * Output -> void 
 * Discription -> function to set filesize of the requested file
 */
void setfilesize(char* fullpath)
{
	struct stat file_stat;
	
	stat(fullpath, &file_stat);
	
	filesize = file_stat.st_size;
}

/*
 * Input -> directory path
 * Output -> fd for the sepcified file for a directory
 * Discription -> Once a directory is specified check for index.html or index.htm
 * 		if any of them exists return fd of them else return -1
 */
int getproperfilefordir(char *_path)
{
		
	char *path_html, *path_htm;
	int filediscripter, length, i = 0;

	length = strlen(_path);

	path_html = malloc(length+20);
	path_htm = malloc(length+20);
	
	while(_path[i]!='\0')
	{
			path_html[i] = _path[i];
			path_htm[i] = _path[i];
			i++;
	}

	if(_path[length-1]!='/')/*check the directory path has a '/' at the end. if not add it*/
	{
		path_html[i] = '/';
		path_htm[i] = '/';
		i++;
	}
	
	path_html[i] = '\0';
	path_htm[i] = '\0';

	/*check for index.html*/
	strcat(path_html,"index.html");
	if((filediscripter=open(path_html,O_RDONLY))!=-1)
	{
		setfilesize(path_html);
		finalpath = path_html;
		return filediscripter;
	}
	
	/*check for index.htm*/
	strcat(path_htm,"index.htm");
	if((filediscripter=open(path_htm,O_RDONLY))!=-1)
	{
		setfilesize(path_htm);
		finalpath = path_htm;
		return filediscripter;
	}

	return -1;
}

/*
 * Input -> complete file for the requested resource
 * Output -> mime type of the specified file
 * Discription -> once file path is specified get mime type
 */
char* getmimetype(char *path)
{
	int i = 0;
	char *cmd = (char*) malloc(500);
	
	sprintf(cmd,"file -i %s|cut -f2 -d\" \"",path);

	char* mimetype = (char*)malloc(300);

	FILE *fp;
	fp = popen(cmd,"r");

	while((mimetype[i]=getc(fp))!=EOF) i++;

	mimetype[i-1] = '\0';

	return mimetype;
}

