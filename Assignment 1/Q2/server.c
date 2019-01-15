#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#define PORT 8115

int main(){
    char *s = "Hello from server";
    int opt = 1;
    struct sockaddr_in sockinfo;
    int sock_fd;

    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }

    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons(PORT);
    sockinfo.sin_addr.s_addr = INADDR_ANY;
    memset(&(sockinfo.sin_zero),'\0',8);

    if( setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if(bind(sock_fd,(struct sockaddr *)&sockinfo,sizeof(sockinfo))<0){
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }


    if(listen(sock_fd,5)<0){
        perror("Failed");
        exit(EXIT_FAILURE);
    }
    int new_fd;
    int info_size = sizeof(sockinfo);
    if((new_fd = accept(sock_fd,(struct sockaddr *)&sockinfo,&info_size))<0){
         perror("Failed");
        exit(EXIT_FAILURE);
    }
    char buffer[1000];
    char rec[1000] = {0};
    read(new_fd,rec,1000);
    printf("%s\n",rec);
    printf("Input the data you have to send to client\n");
    scanf(" %[^\n]",buffer);
    send(new_fd,buffer,sizeof(buffer),0);

}