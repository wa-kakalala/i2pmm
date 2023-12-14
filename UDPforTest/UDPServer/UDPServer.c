#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define RECV_BUF_LEN       (1024)
#define DEFAULT_PORT       (6666)

unsigned short port = DEFAULT_PORT;

char parse_params(int argc, char * argv[]){
    if( argc == 1 ) return 0;
    if( argc > 3 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help")==0 ) {
        printf("-------------------------\r\n");
        printf("Usage:\r\n");
        printf("  .\\UDPServer [-p port]\r\n");
        printf("\r\n");
        printf("<Defualt Port: %d>\r\n",DEFAULT_PORT);
        printf("-------------------------\r\n");
        return -1;
    } 

    if( (argc == 3) && strcmp(argv[1],"-p") == 0  ){
        port = atoi(argv[2]);
        if( port == 0 ) {
            printf("invalid port number !!! \r\n");
            return -1;
        }
    }
    
    return 0;
}

int main(int argc,char *argv[]){
    if( parse_params(argc,argv) ){
        //printf("invalid params !!!\r\n");
        return -1;
    }

    printf("working port: %d\r\n",port);

    int socketid = socket(AF_INET,SOCK_DGRAM,0);
    if( socketid == -1 ) {
        printf("create socket failed !!!\r\n");
        return -1;
    }

    // server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(socketid, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        printf("socket bind fail !!!\n");
        return -1;
    }

    // client info;
    struct sockaddr_in client_addr;
    int client_addr_len =  sizeof(client_addr);

    // communication
    int num = 0;
    char recvbuf[RECV_BUF_LEN];
    char client_ip[16]={0};
    while (1) {
        memset(recvbuf,0,sizeof(recvbuf));
        memset(client_ip,0,sizeof(client_ip));
        num = recvfrom( socketid, recvbuf, sizeof(recvbuf),0,(struct sockaddr * )&client_addr,&client_addr_len);

        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));

        printf("from [%s:%d]: %s",client_ip,ntohs(client_addr.sin_port),recvbuf);
    }
    close( socketid );

    return 0;
}

