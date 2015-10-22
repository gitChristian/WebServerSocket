#include <iostream>
#include <cstdio>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <cstdlib>
#include <cstring>
#include <sys/wait.h> /* for the waitpid() system call */
#include <signal.h> /* signal name macros, and the kill() prototype */
#include <unistd.h>

using namespace std;

void dumpRequest(const int socket, char* requestMessage);
void handleRequest(char* request);
char* getFileName( char* HTTPRequest);

int main(int argc, char *argv[])
{
	if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     //create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        fprintf(stderr,"ERROR, couldn't get socket.\n");

    //bind address of server
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //ip address
    int portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) {
              	fprintf(stderr,"ERROR, couldn't bind.\n");
         		exit(1);
              }

    //listen
    listen(sockfd,5);

    //accept and handle connections one at a time
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    while(1){
	    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	    
	    char requestMessage[10000];
	    dumpRequest(newsockfd, requestMessage);
	    handleRequest(requestMessage);

   		close(newsockfd);//close connection 
    }
    close(sockfd);
}

//part a
//dumps the request to console
//return the HTTP request in char* requestMessage
void dumpRequest(const int socket, char* requestMessage)
{
   	read(socket, requestMessage,10000);
   	printf("Here is the message: %s\n",requestMessage);
}

//part b
void handleRequest(char* request)
{
	char* fileName = getFileName(request);
	printf("Here is the filename: %s\n",fileName);

}

//returns requested filename
char* getFileName(char* HTTPRequest)
{
	char* fileName = strtok(HTTPRequest, " ");
    fileName = strtok(NULL, " ");
    return ++fileName;         
}