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
#define MAX 80

// function for writing to file
void receive_file(char* filename,int sockfd)
{
    char buffer[50];
    int numbytes;
    //TODO: file handling in case that the file doesn't exist
    FILE *receivedFile = fopen(filename,"w"); // open a new file with the same name as the one on the server
    while(buffer[0]!=0)
    {
        numbytes=recv(sockfd,buffer,sizeof(buffer)+1,0);
        fprintf(receivedFile,buffer);
    }
    printf("File received!\n");
    fclose(receivedFile);
}

// get the output from the ls command from server.
void read_ls(int sockfd)
{
    int  numbytes;
    char buf[256];
    while(buf[0]!=0)
    {
        numbytes=recv(sockfd,buf,256,0);
        printf("%s\n",buf);
        //strcat(command_received,buf);
    }

}

// function for communication between server and client
void communicate(int sockfd) 
{ 
    char buff[MAX]; 
    int n;
    int exit = 0; 
    do{ 
        bzero(buff, sizeof(buff));  // here will be the wanted file
        printf("Enter command: "); 
        n = 0; 
        
        while ((buff[n++] = getchar()) != '\n') 
            ;
        char *pointerFile;
        pointerFile = strtok(buff," "); // split the char array after buff
        if(strcmp(buff,"ls\n")==0)
        {
            printf("Sending ls...\n");
            send(sockfd, buff, sizeof(buff),0);
            bzero(buff,sizeof(buff));
            read_ls(sockfd);
            //break; 
            
        }
        else if(strcmp(buff,"exit\n")==0)
        {
            send(sockfd,buff,sizeof(buff),0);
            exit=1;
        }
        else if(strcmp(buff,"get")==0)
        {
            pointerFile = strtok(NULL,""); // get the other half of the array which is the file name
            printf("Sending filename first...\n");
            strcpy(buff,pointerFile); // place the file name inside buffer
            printf("Filename is - %s\n",buff);
            send(sockfd,buff,sizeof(buff),0);
            strtok(buff,"\n"); // it will write a string terminator when it encounter "\n"
            receive_file(buff,sockfd);
           // break; 
        }
        else
        {
            printf("Invalid command, try again...ls,get file or exit\n");
        }
    }while(exit==0); 
} 

int main(int argc, char *argv[])
{
    int sockfd;
    char buf[256];
    char* command_received;
    struct hostent *he;
    struct sockaddr_in their_addr;

    // char *command_ls=argv[2];
    // if(argv[2]==NULL)
    // {
    //     fprintf(stderr,"utilizare argument 2: ls directory\n");
    //     exit(1);
    // }


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
    their_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    memset(&(their_addr.sin_zero), '\0', 8);

    
    if (connect(sockfd, (struct sockaddr *)&their_addr,
                                            sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    // if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     exit(1);
    // }
    // read all data
    printf("--------------------\n");
    printf("Application usage:\n");
    printf("For listing the directory use - ls \n");
    printf("For downloading a file use get filename \n");
    printf("For exit type - exit\n");
    printf("------------------\n");
    communicate(sockfd);
    

    close(sockfd);

    return 0;
}
