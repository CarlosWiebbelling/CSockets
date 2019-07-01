#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void hashCompare(char * hashServer, char * hashClient);
void checksum(char * filename, char ** sum);
void error(const char *msg);

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
       error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0) 
           error("ERROR on binding");

    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, 
           (struct sockaddr *) &cli_addr, 
           &clilen);

    if (newsockfd < 0) 
       error("ERROR on accept");
    else
        printf("Contected with the client...\n");


    while(1) {
        bzero(buffer, 256);

        read(newsockfd, buffer, 255);
        printf("Client says: %s", buffer);

        if (strcmp(buffer, "/write\n") == 0) {

            char * hashServer;
            char hashClient[1024];
            char title[256];
            int currentSize = 0;
            int readLength = 0;
            int totalSize = 0;

            // length archive
            read(newsockfd, &totalSize, sizeof(int));

            // title archive
            read(newsockfd, title, 255);

            // creating copy archive
            FILE * arqv = fopen(title, "w+b");

            // receive chunks of archive
            while (currentSize < totalSize) {
                bzero(buffer, 256);
                readLength = read(newsockfd, buffer, 255);
                fwrite(buffer, readLength, 1, arqv);
                currentSize += readLength;
            }
            fclose(arqv);

            // hash client
            read(newsockfd, hashClient, 1024);

            // hash server
            checksum(title, &hashServer);
            
            // compare the hashes
            hashCompare(hashServer, hashClient);

        } else if (strcmp(buffer, "/close\n") == 0)
            break;
        
        printf("Tip your message: ");

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        write(newsockfd, buffer, strlen(buffer));
    };

    close(newsockfd);
    close(sockfd);

    return 0; 
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void hashCompare(char * hashServer, char * hashClient) {
    printf("Hash on server: %s\n", hashServer);
    printf("Hash on client: %s\n", hashClient);

    if(strcmp(hashServer, hashClient) == 0)
        printf("Transaction successfully\n");
        
    else
        printf("Transaction failed\n");
}

void checksum(char * filename, char ** sum) {
    FILE *checker;
    char cmd[1024];
    *sum = (char *)malloc(1024);
    sprintf(cmd, "md5sum %s", filename);
    checker = popen(cmd, "r");
    fscanf(checker, "%s", *sum);
    fflush(checker);
    fclose(checker);
}