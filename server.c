#define PORT 8779
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<pthread.h>
#define NUM 3 //최대 동시 접속 가능 사람

void *rcv_send(*void data);

void* header_check(void *);

int cl_socket[NUM] = {0,};


int pri_func(){
        for (int i = 0; i<NUM; i++){
                printf("%d", cl_socket[i]);
        }
        printf("\n");
}

int main (){
        int connSock, listenSock;
        struct sockaddr_in s_addr, c_addr;
        socklen_t len;

        pthread_t pthread1;
        int trd_id;

        int cnt = 0;

        listenSock = socket(PF_INET, SOCK_STREAM,0);
        memset(&s_addr,0,sizeof(s_addr));
        s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(PORT);

        if (bind(listenSock,(struct sockaddr *)&s_addr, sizeof(s_addr))== -1){
                printf("can not bind\n");
                return -1;
        }

        if (listen(listenSock,5) == -1){
                printf("listen Fail\n");
                return -1;
        }

        while(1){
                len  = sizeof(c_addr);
                connSock = accept(listenSock,(struct sockaddr*)&c_addr,&len);
                int tmp = 0;
                for (int i = 0; i<NUM; i++){
                        if (cl_socket[i] == 0){
                                cl_socket[i] = connSock;
                                trd_id = pthread_create(&pthread1,NULL,header_check,(void *)connSock);
                                break;
                        }
                        tmp = i+1;
                }
                if (tmp == NUM){
                        printf("connect deny, chat is full\n");
                }

        }
}
struct info{
        int to;
        char text[1000];
        };

void* header_check(void* data){
        int n,i,j;
        char ni[1000];
        int connSock = (int)data;
        struct sendto
        {
                int to;
                char text[1000];
        };


        char nickname[100];
        n = recv(connSock, &nickname, BUFSIZ, 0);
        nickname[n] = '\0';

        printf("guest %s(%d)  login - \n", nickname,connSock);
        char loging[100];
        sprintf(loging, "guest %s(%d) - login\n", nickname, connSock);

        for (int i=0; i<5;i++){
                if(cl_socket[i] != 0){
                        send(cl_socket[i], loging ,BUFSIZ,0);
                }
        }

        while(1){
                n = recv(connSock, &ni, BUFSIZ ,0);
                ni[n] = '\0';
                char snt[100];
                char kn[100];
                strncpy(kn, ni,6);
                char sndcheck[100];
                char nowBuffer[NUM]={48,};
                strncpy(sndcheck, ni,8);
                if (strcmp("exit()", kn) == 0){
                        printf("guest \"%s\" log out-\n", nickname);
                        for (int i = 0; i < NUM; i ++){
                                if (cl_socket[i] == connSock){
                                        cl_socket[i] = 0;
                                        char logout[100];
                                        sprintf(logout, "guest %s - logout\n", nickname);
                                        for (int i=0; i<5;i++){
                                                if(cl_socket[i] != 0){
                                                        send(cl_socket[i], logout ,BUFSIZ,0);
                                                }
                                        }
                                        return -1;

                                }
                        }
                }
                if (strcmp(ni,"now()")==0){
                        int pt = 0;
                        sprintf(snt,"opening socket :: %d %d %d \n",cl_socket[0],cl_socket[1],cl_socket[2]);
                        send(connSock,snt,sizeof(snt),0);
                        continue;
                }
                if (strcmp(sndcheck,"sendto()")==0){
                        struct sendto re_info;
                        recv(connSock,&re_info, sizeof(re_info), 0);
                        printf("prepare sending message to %d\n", re_info.to);
                        char to_msg[100];
                        sprintf(to_msg,"[private_mail]guest \"%s(%d)\" to %d: %s\n",nickname,connSock,re_info.to, re_info.text);

                        int count = 0 ;
                        for (int i=0; i<NUM ;i++){
                                if(cl_socket[i] != 0){
                                        if (cl_socket[i] == re_info.to){
                                                count ++;
                                                send(cl_socket[i],&to_msg ,BUFSIZ,0);
                                        }
                                }

                        }

                        char error_message[100] ;
                        sprintf(error_message, "Sending error or there is no guest %d\n", re_info.to);
                        if (count == 0){
                                send(connSock, &error_message, BUFSIZ, 0);
                                printf("%s", error_message);
                        }
                        else{
                                printf("%s", to_msg);
                                FILE *fp = fopen("chat_log.txt", "a");
                                fprintf(fp, to_msg);
                                fclose(fp);

                        }

                        continue;
                }
                char cknow[100];
                char sntt[NUM];
                int snt_me[100];

                char cklog[100];
                strncpy(cklog, ni, 5);
                if (strcmp("log()", cklog)==0){
                        FILE *rp =fopen("chat_log.txt", "r+");
                        printf("show cat_log\n");
                        char logsnt[100];
                        int m;
                        while(1){

                                m = fgets(logsnt, BUFSIZ  ,rp);
                                if (m == NULL){
                                        break;
                                }
                                printf("%s", logsnt);
                                send(connSock, logsnt, BUFSIZ, 0);
                        }
                        printf("log end\n");
                        fclose(rp);
                        continue;

                }

                sprintf(snt, "guest %s: %s\n",nickname, ni);
                FILE *fp = fopen("chat_log.txt", "a");
                fprintf(fp, snt);
                fclose(fp);
                printf("%s", snt);



                for (int i=0; i<NUM;i++){
                        if(cl_socket[i] != 0){
                                if (cl_socket[i] != connSock){
                                        send(cl_socket[i],&snt ,BUFSIZ,0);
                                }
                        }

                }

        }

}
