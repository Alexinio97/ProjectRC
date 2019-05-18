    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/wait.h>
    #include <signal.h>

    #define MYPORT 5000
    #define BACKLOG 10

    int main(void)
    {
        int sockfd, new_fd;
        struct sockaddr_in my_addr;
        struct sockaddr_in their_addr;
        int sin_size;
        int yes=1;

        // linux ls command variables
        FILE *fp;
        char* command;
        char* data;

        printf("Server started...");

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }

        if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(MYPORT);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        memset(&(my_addr.sin_zero), '\0', 8);

        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
            perror("bind");
            exit(1);
        }

        if (listen(sockfd, BACKLOG) == -1) {
            perror("listen");
            exit(1);
        }
        
        while(1) {  
            sin_size = sizeof(struct sockaddr_in);
            if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1) {
                perror("accept");
                continue;
            }
            printf("server: conexiune de la: %s\n",inet_ntoa(their_addr.sin_addr));
            printf("Wating for a directory...\n");
           
            // client connected so we will send a simple file
            recv(new_fd,command,256,0);
            printf("Command received from client: %s\n",command);
            fp=popen(command,"r");
            if(!fp)
            {
                fprintf(stderr,"Pipe could not be opened.");
                return 1;
            }
            while(1)        // another while true for sending data
            {

                unsigned char buff[256]={0};
                int nread = fread(buff,1,256,fp);
                printf("Bytes read and sent %d \n", nread);        

                /* If read was success, send data. */
                if(nread > 0)
                {
                    printf("Sending \n");
                    if(send(new_fd, buff, nread,0) ==-1)
                    {
                        printf("Error writing to file.");
                        return 1;
                    }
                }

                if (nread < 256)
                {
                    if (feof(fp))
                        printf("End of file\n\n");
                    if (ferror(fp))
                        printf("Error reading\n");
                    break;
                }
            }

            // if (send(new_fd, data, 14, 0) == -1)
            //     perror("send");
            close(new_fd);
        }

        return 0;
    }
