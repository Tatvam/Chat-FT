#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8115
#define Max_Client 10

char *s = "Hello from server";
struct sockaddr_in sockinfo;
struct timeval tv;
int sock_fd;
int opt = 1;
int client_list[Max_Client] = {0};

int list_socket(int *sock_fd){

    if((*sock_fd = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Socket()");
        return -1;
    }
    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons(PORT);
    sockinfo.sin_addr.s_addr = INADDR_ANY;
    memset(&(sockinfo.sin_zero),'\0',8);

    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))<0)
    {
        perror("setsockopt failed");
        return -1;
    }

    if(bind(*sock_fd,(struct sockaddr *)&sockinfo,sizeof(sockinfo))<0){
        perror("Bind()");
        return -1;
    }

    if(listen(*sock_fd,10)<0){
        perror(Listen());
        return -1;
    }
    
    printf("Accepting connections on port %d.\n", (int)PORT);
}
void build_fd_sets(fd_set *readfds,fd_set *writefds,fd_set *exceptfds){

    FD_ZERO(readfds);
    FD_SET(STDIN_FILENO,readfds);
    FD_SET(sock_fd,readfds);

}
int main(){
  
    if(list_socket(&sock_fd)!=0){
        exit(EXIT_FAILURE);
    }
    // Non Blocking STD_IN 
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;

    int mx_fd = sock_fd;

    tv.tv_sec = 10;
    tv.tv_usec = 500000;

    printf("Waiting for incoming connections.\n");

    while(1){
        build_fd_sets(&readfds,&writefds,&exceptfds);
        int new_fd;
        int info_size = sizeof(sockinfo);

        int sel = select(mx_fd+1,&readfds,NULL,NULL,&tv);

        if(sel == -1){
            perror("Select()");
        }else if(sel == 0){
            printf("Timeout Occured");
        }else{
            if(FD_ISSET(sock_fd,&readfds)){
                if((new_fd = accept(sock_fd,(struct sockaddr *)&sockinfo,&info_size))<0){
                    perror("Accept()");
                    exit(EXIT_FAILURE);
                }
                 printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_fd , inet_ntoa(sockinfo.sin_addr) , ntohs (sockinfo.sin_port)); 

                if(send(new_fd,s,sizeof(s),0)!=strlen(s)){
                    perror("Send()");
                }

                 puts("Welcome message sent successfully");   

                 int i;
                 for(i = 0;i<Max_Client;i++){
                     if(client_list[i]==0){
                         client_list[i]=new_fd;
                         break;
                     }
                 }
             }

             if()
         }
     }

    }

