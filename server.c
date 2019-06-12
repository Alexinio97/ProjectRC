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
#define MAX 80

char* serverDir = "/home/alan271265/ServerDir/";
// where the wanted file is stored


void send_file(char filename[],int new_fd)
{
    char full_path[30]={0};
    printf("File is - %s\n",filename);
    // // construct full path to file location
    // strcat(full_path,serverDir);
    // printf("Full path - %s\n",full_path);
    // strcat(full_path,filename);
    // printf("Full command 2- %s\n",full_path);

    sprintf(full_path,"%s%s",serverDir,filename); // construct full path using serverDir and the filename received
    printf("Command full is - %s\n",full_path);
    
    char ch;
    FILE *file = fopen(full_path,"r");
    if(file == NULL)
    {
        perror("Failed: ");
    }
    char buffer[100]={0};
    int bytes_read;
    printf("Opened with succes.\n");
    while (!feof(file))  
    {
        if ((bytes_read = fread(&buffer, 1, sizeof(buffer), file)) > 0) // as long as there are bytes to read keep sending them
        {
            send(new_fd, buffer, bytes_read, 0);
            printf("Inside if.\n");
        }
        else
        {
            printf("End of file...\n");
            break;
        }
    }
    printf("File sent.");
    fclose(file);
}

// function that sends the output of ls
void send_dir_ls(char* command,int new_fd)
{
    char full_command[26]={0};
    printf("Command is - %s\n",command);
    // adding ls to our full buffer and the server dir
    strcat(full_command,"ls ");
    printf("Full command - %s\n",full_command);
    strcat(full_command,serverDir);
    printf("Full command 2- %s\n",full_command);

    printf("Command full is - %s\n",full_command);
    FILE *fp=popen(full_command,"r");
    if(!fp)
    {
        fprintf(stderr,"Pipe could not be opened.\n");
        exit(1);
    }
    char buff[256] = { 0 };
    while(fgets(buff,256,fp)) // read from pipe command  256 bytes at a time
    {
        printf("%s\n",buff);
        if(send(new_fd,buff,256,0) == -1) 
        {
            printf("Error sending data...");
            exit(1);
        }
        if(feof(fp))
        {
            printf("End of file...");
            break;
        }
    }
    close(new_fd);
    fclose(fp);
}

// Function designed for chat between client and server. 
void communicate(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); // reinitialize the buffer , empty it's value
  
        // read the message from client and copy it in buffer 
        recv(sockfd, buff, sizeof(buff),0); 
        // print buffer which contains the client contents 
        printf("From client: %s\n", buff); 
        
        if(strcmp(buff,"ls\n")==0)
        {
            bzero(buff, MAX);
            send_dir_ls("ls",sockfd);
            printf("%s\n",buff);
            
            break;
        }
        else if(strcmp(buff,"exit\n")==0)
        {
            exit(1); //stop the server
        }
        else
        {
            //bzero(buff, MAX);
            printf("Sending file.\n");
            strtok(buff,"\n");  // it will write a string terminator when it encounter "\n"        
            send_file(buff,sockfd);
            break;
        }   
    } 
}






int main(void)
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    int yes=1;

    // linux ls command variables
    char command[40];
    char* data;
    char file_wanted[40]={0};
    

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
        
        communicate(new_fd); // functions that handles the client requests (ls or filenames)
        close(new_fd);
    }

    return 0;
}
