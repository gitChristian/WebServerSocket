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
void handleRequest(char* request, int socket);
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
	    handleRequest(requestMessage, newsockfd);

   		close(newsockfd);//close connection 
    }
    close(sockfd);
}


void header(int socket, int status, char* fileName, int contentLength, char* request)
{
    write(socket, "HTTP/1.1 ", strlen("HTTP/1.1 "));

	if (status == 200)
	    write(socket, "200 OK", strlen("200 OK"));
	else if (status == 400)
	    write(socket, "Bad Request", strlen("Bad Request"));
	else if (status == 404)
	    write(socket, "404 Not Found", strlen("404 Not Found"));
	else if (status == 500)
	    write(socket, "500 Internal Server Error", strlen("500 Internal Server Error"));
	else 
	    write(socket, "Unknown error type", strlen("Unknown error type"));

    char content[10000];
	if(strstr(request, ".html") != NULL)
        strcpy(content, "text/html; charset=UTF-8\n");
	else if(strstr(request, ".gif") != NULL)
        strcpy(content, "image/gif\n");
	else if(strstr(request, ".jpeg") != NULL)
        strcpy(content, "image/jpeg\n");
	else if(strstr(request, ".jpg") != NULL)
        strcpy(content, "image/jpg\n");
	/*else {
	    fprintf(stderr,"ERROR, not supported\n");
		exit(1);
	}*/

    //content 
    char length[10000];
    write(socket, "\nContent-Language: en-US\n", 25);
    write(socket, "Content-Type: ",14);
    write(socket, content, strlen(content));
    sprintf(length, "Content-Length;%d", contentLength);
    write(socket, length, strlen(length));
    write(socket, "\nConnection: keep-alive\n\n", 25);
}

//part a
//dumps the request to console
//return the HTTP request in char* requestMessage
void dumpRequest(const int socket, char* requestMessage)
{
    int check = read(socket, requestMessage,10000);
    if (check < 0){
    perror("ERROR READING");
    exit(1);
	}
   	printf("Here is the message: %s\n",requestMessage);
}

//part b
void handleRequest(char* request, int socket)
{
	char* fileName = getFileName(request);

	//open file and get file size
	FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
    	return;
        /*header(socket, 404, "text/html", 0, fileName);
        printf("Failed to open '%s'", fileName);    */
    }

	fseek(fp, 0L, SEEK_END); 
    int fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
        
    header(socket, 200, fileName, fileSize, fileName);

    void* fileBuffer = malloc(sizeof(char) * fileSize + 100);
    fread(fileBuffer, sizeof(char), fileSize, fp);
    
    write(socket,fileBuffer,fileSize);
    free(fileBuffer);
    fclose(fp);
}

//returns requested filename
char* getFileName(char* HTTPRequest)
{
	char* fileName = strtok(HTTPRequest, " ");
    fileName = strtok(NULL, " ");
    return ++fileName;         
}