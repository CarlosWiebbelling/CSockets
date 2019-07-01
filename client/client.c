#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define ARQVSEND "video_eden.mp4"

void checksum(char * filename, char ** sum);
void error(const char *msg);

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    else
        printf("Contected with the server...\n");

    char buffer[256];

    while(1) {
        printf("Tip your message: ");

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        write(sockfd, buffer, strlen(buffer));
        
        if (strcmp(buffer, "/write\n") == 0) {
            char * hashClient;
            FILE * arqv = fopen(ARQVSEND, "r+b");

            // total length of archive
            fseek(arqv, 0, SEEK_END);
            int totalSize = ftell(arqv);
            rewind(arqv);

            // sending length
            write(sockfd, &totalSize, sizeof(totalSize));

            // sending title
            write(sockfd, ARQVSEND, sizeof(ARQVSEND));

            int currentSize = 0;
            int readLength = 0;

            // sending chunks of archive
            while (currentSize < totalSize) {
                bzero(buffer, 256);
                readLength = fread(buffer, 1, sizeof(buffer), arqv);
                write(sockfd, buffer, readLength);
                currentSize += readLength;
            }
            fclose(arqv);

            // sending hash checksum
            checksum(ARQVSEND, &hashClient);
            write(sockfd, hashClient, 1024);
        } else if (strcmp(buffer, "/close\n") == 0)
            break;

        bzero(buffer, 256);
        read(sockfd, buffer, 255);
        printf("Server says: %s", buffer);
        
    };

    close(sockfd);
    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(0);
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
