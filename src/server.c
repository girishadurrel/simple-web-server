
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include "headers/filefunc.h"
#include "headers/stringfunc.h"
#include "headers/headerhandlers.h"
#include "headers/globals.h"

#define PORT "9999" /*Port number of the webserver*/
#define QLEN 10 /*Number of clients that could wait in the server queue*/
#define REQUESTMAXLENGTH 1024 /*Maximum length of the request from the client*/

/*Some constants neded for the code*/
#define ERROR 2
#define OK 1
#define TRUE 1
#define FALSE 0

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void)
{
	int sd, status, header_size, val = 1, i = 0, filediscripter, num, sent;
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	time_t rawtime;
	char buf[REQUESTMAXLENGTH];
	char *resource, *badreq_header, *notfndreq_header, *mime, *okreq_header;
	struct sigaction sa;

	char *request = (char*)malloc(REQUESTMAXLENGTH);
	
	memset(&hints,0,sizeof(hints));

 	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((status = getaddrinfo(NULL,PORT,&hints,&res))!=0)
	{
		printf("error::get addr\n");
		return 1;	
	}
	
	if((sd = socket(res->ai_family,res->ai_socktype,res->ai_protocol))==-1)
	{
		printf("error::socket\n");
		return 1;
	}

	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if((bind(sd,res->ai_addr,res->ai_addrlen))!=0)
	{
		printf("error::bind\n");
		return 1;
	}

	if(listen(sd,QLEN)==-1)
	{
		printf("error::listen\n");
		return 1;
	}

	addr_size = sizeof their_addr;

	time(&rawtime);

	printf("Girisha's Web Server :: Status OK\nUP and RUNNING in Port : 9999\nUp Time : %s",ctime(&rawtime));

	sa.sa_handler = sigchld_handler; // reap all dead processes
    	sigemptyset(&sa.sa_mask);
    	sa.sa_flags = SA_RESTART;
    	if (sigaction(SIGCHLD, &sa, NULL) == -1) 
	{
        	perror("sigaction");
        	exit(1);
    	}
	
	while(TRUE)
	{
		/*a new client is accepted and the socket discripter is new_fd*/
		if((new_fd = accept(sd, (struct sockaddr *)&their_addr, &addr_size))==-1)
		{	
			printf("error::%d\n",new_fd);
			return 1;
		}

		if(!fork()) /*child procees*/
		{
			close(sd);
			num = recv(new_fd,&buf,REQUESTMAXLENGTH,0); /*recive request from client*/
		
			/*from request header extract only the GET RESOURCE VERSION */
			while(buf[i]!='\r')
			{
				request[i] = buf[i];
				i++;	
			}
			request[i] = '\0';

			/*validate request, if request is not valid send BAD REQUEST*/
			if((resource = validaterequest(request))==NULL) /*check for proper request*/
			{
				filediscripter = getfd("badrequest.html",ERROR);		
		
				badreq_header = getheader(1,"text/html");
				header_size = strlen(badreq_header);
	
				if((sent = send(new_fd,badreq_header,header_size,0))==-1) /*send header*/
				{
					printf("error::send\n");
					exit(1);
				}
			
				sendfile(new_fd,filediscripter,0,filesize);
		
				exit(0);
			}
		
			/*find file, if the resource is missing send NOT FOUND*/
			if((filediscripter = getfd(resource,OK))==-1) /*check for file*/
			{
				filediscripter = getfd("notfound.html",ERROR);
		
				notfndreq_header = getheader(2,"text/html");
				header_size = strlen(notfndreq_header);
				
				if((sent = send(new_fd,notfndreq_header,header_size,0))==-1) /*send header*/
				{
					printf("error::send\n");
					exit(1);
				}

                                sendfile(new_fd,filediscripter,0,filesize);

				exit(0);
			}

			/*Valid Request + Resource Found, send file to client*/			
			mime = getmimetype(finalpath);

			okreq_header = getheader(3,mime);
			header_size = strlen(okreq_header);

			if((sent = send(new_fd,okreq_header,header_size,0))==-1) /*send header*/
                        {
                        	printf("error::send\n");
                                exit(1);
                        }
			
			sendfile(new_fd,filediscripter,0,filesize);
			
			close(new_fd);
			exit(0);
		}

		close(new_fd);
	}

	return 0;
}
