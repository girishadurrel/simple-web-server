
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/*Define Function Prototypes*/
char* validaterequest(char*);

int isvalidmethod(char*);

int isvalidversion(char*);

int checkstring(char*,char*);

/*
 * Input -> request from the client
 * Ouput -> requested resource from the client
 * Description -> the request from the client is checked so that it is in the proper format and the 
 *			resource is requested returnd if request is not in proper format return  NULL
 */
char* validaterequest(char *req)
{
	char *method, *request, *version, *token;
	int i = 0;

	while(TRUE)
	{	
		/*tokenize the input request*/
		if(i==0) token = strtok(req," ");
		
		else token = strtok(NULL," ");
	
		if(token == NULL) break;
				
		if(i==0) /*validate the method GET*/
		{	
			method = token;
			if(!(isvalidmethod(method))) return NULL;
			else 
			{
				i++;
				continue;
			}
		}
		
		else if(i==1) 
		{
			request = token;
			i++;
		}
		
		else if(i==2) /*validate the HTTP version*/
		{
			version = token;
			if(!(isvalidversion(version))) return NULL;
			else 
			{
				i++;
				continue;
			}
		}
		
		else 
			return NULL;

	}

	if(request!=NULL && i==3) 
		return request;
	else	
		return NULL;
}

/*
 * Input -> method part of the request from the client
 * Ouput -> 1 if method is GET else return 0
 * Description -> the method part of the request is validated using this method
 */
int isvalidmethod(char* method)
{
	char *syntax = "GET";
	return checkstring(method,syntax);	
}

/*
 * Input -> version part of the request from the client
 * Ouput -> 1 if version is HTTP/1.1 or HTTP/1.0 else return 0
 * Description -> the version part of the request is validated using this method
 */
int isvalidversion(char* version)
{
	char* http1 = "HTTP/1.1";
	char* http2 = "HTTP/1.0";

	if(!(checkstring(version,http1)))
	{
		return checkstring(version,http2);
	}

	return TRUE;
}

/*
 * Input -> two strings
 * Ouput -> 1 if both strings are identical else return 0
 * Description -> a string matching method
 */
int checkstring(char* str,char* syntax)
{
	int i=0;
	while(syntax[i]!='\0')
	{
		if(syntax[i]=='\0') return FALSE;
		if(str[i]!=syntax[i]) return FALSE;
		i++;	
	}
		
	if(str[i]=='\0')return TRUE;
	else return FALSE;
}
