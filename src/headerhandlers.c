
#include <stdio.h>
#include <stdlib.h>

#include "headers/stringfunc.h"
#include "headers/globals.h"

/*Define Function Prototypes*/
char* getheader(int,char*);

char* getspecialheader(char*);

/*
 * Input -> type of header and the mime type of the requested resource
 * Output -> the generated mime
 * Discription -> generate the header using the mime and length of the requested resource
 */
char* getheader(int type,char *mime)
{

	char *version = "HTTP/1.1";
	char *serverinfo = "Server: Girisha's Web Server v1.0";
	char *contenttype = "Content-Type: ";
	char *contentlength = "Content-Length: ";
	char *CRFL = "\r\n"; 

	char *badreq, *notfoundreq, *okreq, *content;
	
	char *header = malloc(1024*2);

	int ret;
	
	switch(type)
	{
		case 1:
			badreq = "400 Bad Request";
			ret = sprintf(header,"%s %s\r\n%s\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",version,badreq,serverinfo,filesize,mime);
			break;		
		case 2:
			notfoundreq = "404 Not Found";
			ret = sprintf(header,"%s %s\r\n%s\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",version,notfoundreq,serverinfo,filesize,mime);
			break;
		case 3:
			okreq = "200 OK";
			ret = sprintf(header,"%s %s\r\n%s\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",version,okreq,serverinfo,filesize,mime);
			break;
	}
	
	return header;
}

/*
 * Input -> path of the directory (absoulte to the contents directory)
 * Output -> header to browser to move permenantly
 * Discription -> when a browser requests for a directory without a '/' at the end instruct 
 *			the browser to move permenenlty to the directory location with a '/'
 */
char* getspecialheader(char* location)
{
	char *header = (char*) malloc(500);
	char *partinfo1 = "HTTP/1.1 301 Moved Permanently\r\nLocation: ";
	char *partinfo2 = "\r\nConnection: Close\r\n\r\n";
	
	int ret;

	ret = sprintf(header,"%s%s%s",partinfo1,location,partinfo2);
	
	return header;	
}
