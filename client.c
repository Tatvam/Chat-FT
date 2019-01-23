// Code For File Transfer between client and server

// Libraries Included

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <pthread.h>

#define PORT 8080
#define Max_Size 1000


// void *send_data(int *sock_fd){

//     char buffer[Max_Size];
//     while(1){
//         bzero(buffer,Max_Size);

//         //printf("Write message to server");
//         scanf("%s",buffer);

//         int send_data = send(*sock_fd,buffer,sizeof(buffer),0);
//         if(send_data<0){
//             perror("Sending Failure");
//             exit(1);
//         }
//     }

// }

void *recieve(void *sock_fd){

    int soc_fd = *(int *) sock_fd;

    char buffer[Max_Size];
    bzero(buffer,Max_Size);
    char cache[Max_Size];
    bzero(cache,Max_Size);
    int rev_data;
    while(1) {

       // printf(".\n");  
        
        bzero(buffer, Max_Size);
        int n = recv(soc_fd, buffer, Max_Size-1, MSG_DONTWAIT); 

        int len = strlen(buffer);

        if (len > 0 && strcmp(buffer, cache) != 0) {
            printf("%s", buffer);
            bzero(cache, Max_Size);
            memcpy(cache, buffer, len);
        }

        sleep(2);
    }
        close(soc_fd);
        return NULL;
}

int main(int argc, char *argv[]){
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

    pthread_t rec;
    pthread_create(&rec,NULL,recieve,(void *)&sock_fd);

    char buffer[Max_Size];

    while(1){
        printf("> ");
        bzero(buffer,Max_Size);
        fgets(buffer,Max_Size-1,stdin);

        int n = write(sock_fd,buffer,strlen(buffer));
        if(n < 0){
            break;
        }
    }
    printf("Lost connection to server\n");
    pthread_exit(NULL);
    close(sock_fd);
    return 0;
    
    
}