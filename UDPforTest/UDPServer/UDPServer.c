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

    int socketid = socket(PF_INET,SOCK_DGRAM,0);
    if( socketid == -1 ) {
        printf("create socket failed !!!\r\n");
        return -1;
    }

    // server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // client info;
    struct sockaddr_in client_addr;
    int client_addr_len =  sizeof(client_addr);

    // communication
    int num = 0;
    char recvbuf[RECV_BUF_LEN];
    while (1) {
        memset(recvbuf,0,sizeof(recvbuf));
        num = recvfrom( socketid, recvbuf, sizeof(recvbuf),0,(struct sockaddr * )&client_addr,&client_addr_len);
        printf("%s",recvbuf);
    }

    return 0;
}

