    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
    #include <netdb.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>

    #define PORT 5000

    #define MAXDATASIZE 2048

    int main(int argc, char *argv[])
    {
        int sockfd, numbytes;
        char buf[256];
        char* command_received;
        struct hostent *he;
        struct sockaddr_in their_addr;

        char* directory=argv[2];
        if(argv[2]==NULL)
        {
            fprintf(stderr,"utilizare argument 2: ls directory\n");
            exit(1);
        }

        if (argv[1] == NULL) {
            fprintf(stderr,"utilizare argument: client host\n");
            exit(1);
        }

        if ((he=gethostbyname(argv[1])) == NULL) {
            perror("gethostbyname");
            exit(1);
        }

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }

        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(PORT);
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);
        memset(&(their_addr.sin_zero), '\0', 8);

        if (connect(sockfd, (struct sockaddr *)&their_addr,
                                              sizeof(struct sockaddr)) == -1) {
            perror("connect");
            exit(1);
        }

        //send the directory and command to server
        if(send(sockfd,directory,256,0)==-1)
        {
            perror("send error directory");
            exit(1);
        }
        // if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        //     perror("recv");
        //     exit(1);
        // }
        // read all data
        while((numbytes=recv(sockfd,buf,256,0)!=0))
        {
            printf("%s\n",buf);
            strcat(command_received,buf);
        }

        //buf[numbytes] = '\0';

        
        printf("Primit: %s",command_received);

        close(sockfd);

        return 0;
    }
