#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#define PORT 8115

int main(){
    //char *s = "Hello from server";
    struct sockaddr_in sockinfo;
    int sock_fd;

    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }

    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons(PORT);
    inet_aton("127.0.0.1",&(sockinfo.sin_addr));
    memset(&(sockinfo.sin_zero),'\0',8);


    if(connect(sock_fd,(struct sockaddr *)&(sockinfo),sizeof(sockinfo))<0){
         perror("Failed");
        exit(EXIT_FAILURE);
    }

    char buffer[1000];
    char rec[1000];
    int len = read(sock_fd,rec,1000);
    rec[len]='\0';
    printf("%s\n",rec);
    printf("Request to server\n");
    while(1){
   // sleep(4);
    scanf(" %[^\n]",buffer);
    sleep(4);
    send(sock_fd,buffer,sizeof(buffer),0);
    int len = read(sock_fd,rec,1000);
    rec[len]='\0';
    printf("%s\n",rec);
    }
}
