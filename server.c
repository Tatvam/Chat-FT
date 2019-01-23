// Code For File Transfer between client and server

// Libraries Included

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <dirent.h>

#define Max_Client 30
#define Max_Size 1000

struct client_details{
    int sockno;
    char ip[INET_ADDRSTRLEN];
};
  
struct sockaddr_in sockinfo;
struct dirent *myfile;
pthread_attr_t pthread_attr;
int client[Max_Client];
struct client_details cd[Max_Client];
DIR *mydir;
int opt = 1;
int PORT;
pthread_mutex_t lock;
char shared_buffer[Max_Size];
pthread_t new_client;

int enable_listen(int *server_fd,int PORT){

    if((*server_fd = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Socket()");
        return -1;
    }

    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons(PORT);
    sockinfo.sin_addr.s_addr = INADDR_ANY;
    memset(&(sockinfo.sin_zero),'\0',8);

    if(setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))<0)
    {
        perror("setsockopt failed");
        return -1;
    }

    if(bind(*server_fd,(struct sockaddr *)&sockinfo,sizeof(sockinfo))<0){
        perror("Bind()");
        return -1;
    }

    if(listen(*server_fd,30)<0){
        perror("Listen()");
        return -1;
    }
    
    printf("Accepting connections on port %d.\n", (int)PORT);
    return 0;
}

// void *send_to_all(int *client_fd){

//     while(1){
//         char buffer[Max_Size];
//         printf("Message to clients");
//         scanf("%s\n",buffer);

//         int size_data = send(*client_fd,buffer,sizeof(buffer),0);
//         if(size_data<0){
//             perror("Sendind failure");
//             exit(1);
//         }
//     }

// }

// void *recieve(int *client_fd){

//     char buffer[Max_Size];

//     int i;
//     for(int i = 0;i<Max_Client;i++){
//         if(client[i]==(*client_fd)){
//             break;
//         }
//     }
//     int size_data;
//     while((size_data=recv(*client_fd,buffer,sizeof(buffer),0)) > 0){

//         buffer[size_data]='\0';
    
//         printf("Message recieved from %s:%d",cd[i].ip,cd[i].sockno);
//         printf("%s\n",buffer);

//     }
//     // if(size_data==-1){
//     //         perror("Receiving");
//     //         exit(1);
//     // }
    
//     if(size_data ==0){
//         client[i]=-1;
//         printf("Host disconnected , ip %s , port %d \n" , cd[i].ip , cd[i].sockno);
//         pthread_kill(*client_fd,0);
//         pthread_exit(NULL);
//         close(*client_fd);
//     }
// }

void *handle_message(void *cli_fd){
    // Send to all , Send to perticular Client, Receive for Client
    int client_fd = *(int *)cli_fd;
    int n,i;
    pthread_mutex_lock(&lock);
    for(i = 0;i<Max_Client;i++){
        if(client[i]==(client_fd)){
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    char buffer[Max_Size], message[Max_Size], command[Max_Size], msg_token;

    bzero(buffer, Max_Size);
    sprintf(buffer, "Welcome to the Chat Server");
    n = write(client_fd, buffer, strlen(buffer));
    if (n < 0) return NULL;
    printf("client %d has joined\n", i);   

    pthread_mutex_lock(&lock);
    bzero(shared_buffer, Max_Size);
    sprintf(shared_buffer, "Client %d has joined\n", i);
    pthread_mutex_unlock(&lock);

    while(1){
        bzero(buffer,Max_Size);
        int recv_data = recv(client_fd,buffer,Max_Size-1,MSG_DONTWAIT); /* NON-BLOCKING READ */   
        if(recv_data < 0){
            //printf("Write Message to client");
            n = write(client_fd,shared_buffer,strlen(shared_buffer));
            if(n < 0){
                printf("Client %d has disconnected\n", i);
                pthread_mutex_lock(&lock);
                bzero(shared_buffer, Max_Size);
                sprintf(shared_buffer, "Client %d has disconnected", i);
                pthread_mutex_unlock(&lock);
                break;
            }
        }
        else{
            printf("Client %d: %s",i,buffer);

            pthread_mutex_lock(&lock);
            bzero(shared_buffer, Max_Size);
            sprintf(shared_buffer, "Client %d: %s", i, buffer);
            pthread_mutex_unlock(&lock);
        } 
         sleep(2);  
    }
    printf("Client %d has exited\n", i);
    pthread_mutex_lock(&lock);
    client[i]=-1;
    pthread_mutex_unlock(&lock);
    close(client_fd);
    return NULL;
}

void handle_new_connection(int *server_fd){

    // Client details such as ip and port
    struct sockaddr_in client_info;
    memset(&client_info,0,sizeof(client_info));
    socklen_t client_len = sizeof(client_info);
    int new_client_fd = accept(*server_fd,(struct sockaddr *)&client_info,&client_len); 
    if(new_client_fd < 0){
        perror("accept()");
        exit(1);
    }
    char client_ipv4[INET_ADDRSTRLEN];
    // convert binary to test form
    inet_ntop(AF_INET,&client_info.sin_addr, client_ipv4, INET_ADDRSTRLEN);

    printf("Incoming connection from %s:%d.\n", client_ipv4, client_info.sin_port);

    int i;
    for(i = 0;i<Max_Client;i++){
        if(client[i]==-1){
            client[i]=new_client_fd;
            cd[i].sockno = client_info.sin_port;
            strcpy(cd[i].ip,client_ipv4);
            break;
        }
    }


    if(pthread_create(&new_client,NULL,(void *)&handle_message, (void *)&new_client_fd)!=0){
        perror("pthread_create()");
        exit(1);
    }
}

int main(int argc, char *argv[]){

    int server_fd, acc_fd;

    int i;
    for(i = 0;i<Max_Client;i++){
        client[i]=-1;
    }
    memset(&cd,0,sizeof(cd));

    PORT = argc > 1 ? atoi(argv[1]) : 0;
    if(!PORT){
        printf("Enter Port :");
        scanf("%d",&PORT);
    }

    if(enable_listen(&server_fd,PORT)<0){
        perror("Listen()");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for incoming connections.\n");
     
    // initialize  pthread attribute to create detached threads

    if(pthread_attr_init(&pthread_attr) !=0){
        perror("Pthread_attr_init()");
        exit(1);
    }

    if(pthread_attr_setdetachstate(&pthread_attr,PTHREAD_CREATE_DETACHED)!=0){
        perror("pthread_attr_setdetachstate()");
        exit(1);
    }

    while(1){

        handle_new_connection(&server_fd);     

    }
    pthread_join(new_client, NULL);
    pthread_mutex_destroy(&lock);
    close(server_fd);
    return 0;

}