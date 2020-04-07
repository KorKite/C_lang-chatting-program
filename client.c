#define PORT 8779
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<pthread.h>

#define IPADDR "127.0.0.1"
void *receiv(void*);
void *sending(void*);

void now(int *lnow){

        printf("%d",lnow);
}
char who[] = "Enter the socket number : \n";
char rcvBuffer[BUFSIZ];
int main(){
        int c_socket;
        struct sockaddr_in c_addr;
        int len;

        char* temp;
        char* info;


        int n;
        int n_left, nrecv;

        pthread_t pthread1,pthread2;
        int trd_id1,trd_id2;

        c_socket = socket(PF_INET, SOCK_STREAM,0);
        memset(&c_addr,0,sizeof(c_addr));
        c_addr.sin_addr.s_addr = inet_addr(IPADDR);
        c_addr.sin_family = AF_INET;
        c_addr.sin_port = htons(PORT);
        if(connect(c_socket,(struct sockaddr *)&c_addr,sizeof(c_addr))== -1){
                printf("connect fail\n");
                close(c_socket);


        }
        int status;
                trd_id2 = pthread_create(&pthread2, NULL, receiv,(int *)c_socket);
                if (trd_id1 <0){
                        printf("thread create 1 error");
                        exit(0);
                }
                if (trd_id2 < 0){
                        printf("thread create 2 error");
                        exit(0);
                }
                struct info{
                        int to;
                        char text[1000];
                };
                struct info ho;
                char lev[100];

                printf("\n=======================\n");
                printf("Welcome to CO'nect\n\n");
                printf("to see log -> log()\n");
                printf("to exit -> exit()\n");
                printf("to clear window -> clear()\n");
                printf("to send to someone -> sendto()\n");
                printf("======================== \n\n");
                printf("enter your nickname:");
                while(1){
                        scanf("%s", lev);

                        if (strcmp(lev, "clear()") == 0){
                                system("clear");
                                printf("line cleaned\n");
                                continue;
                        }
                        send(c_socket, &lev, sizeof(lev), 0);
                        if (strcmp(lev, "sendto()")==0){
                                printf("send to who?");
                                scanf("%d",&ho.to);
                                printf("Enter the message :");
                                scanf("%s",ho.text);
                                printf("to %d, message: %s\n==complete==\n\n", ho.to, ho.text);
                                send(c_socket,&ho,sizeof(ho),0);
                                continue;
                        }
                        if (strcmp(lev, "exit()\n") == 0){
                                sleep(2);
                                exit(0);
                                system("^C");
                        }

                }
                printf("ending\n");


}
void *receiv(void *data){
        int n;
        int c_socket;

        c_socket = (int*)data;
        while(1){
                if ((n = recv(c_socket, &rcvBuffer, sizeof(rcvBuffer),0))>0){
                        rcvBuffer[n] = '\0';
                        if (strcmp(rcvBuffer, "\n")){
                                printf("%60s",rcvBuffer);
                        }
                }
        }
        printf("thread end in rev\n");
}

