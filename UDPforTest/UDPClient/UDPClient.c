#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SEND_BUF_LEN       (1024)
#define DEFAULT_IP         "127.0.0.1"
#define DEFAULT_PORT       (6666)
#define DEFAULT_SEND_NUM   (100 )

unsigned short server_port = DEFAULT_PORT;
char * server_ip = DEFAULT_IP;
unsigned short send_num = DEFAULT_SEND_NUM;

char parse_params(int argc, char * argv[]){
    if( argc == 1 ) return 0;
    if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help")==0 ) {
        printf("-------------------------\r\n");
        printf("Usage:\r\n");
        printf("  .\\UDPClient [-ip ip_addr] [-p port] [-n send_num]\r\n");
        printf("\r\n");
        printf("<Defualt Ip      : %s >\r\n",DEFAULT_IP);
        printf("<Defualt Port    : %d >\r\n",DEFAULT_PORT);
        printf("<Defualt Send_Num: %d >\r\n",DEFAULT_SEND_NUM);
        printf("-------------------------\r\n");
        return -1;
    }

    for(int i=1;i<argc;i++){
        if( strcmp(argv[i],"-p") == 0 ){
            server_port = atoi(argv[i+1]);
            i++;
            if( server_port == 0 ) {
                printf("invalid port number !!! \r\n");
                return -1;
            }
        }else if(strcmp(argv[i],"-ip") == 0 ){
            server_ip = argv[i+1];
            i++;
        }else if( strcmp(argv[i],"-n") == 0 ){
            send_num = atoi(argv[i+1]);
            i++;
            if( send_num == 0 ) {
                printf("invalid send_num number !!! \r\n");
                return -1;
            }
        }else{
            printf("invalid param !!! \r\n");
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

    printf("working ip      : %s\r\n",server_ip);
    printf("working port    : %d\r\n",server_port);
    printf("working send num: %d\r\n",send_num);

    int socketid = socket(PF_INET,SOCK_DGRAM,0);
    if( socketid == -1 ) {
	perror("create socket failed !!!");
	return -1;
    }

    // server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(server_port);
    inet_pton(AF_INET,server_ip,&server_addr.sin_addr.s_addr);

    // communication
    char sendbuf[SEND_BUF_LEN];

    for( int count = 1; count <= send_num ; count ++ ){
        memset(sendbuf,0,sizeof(sendbuf));
        sprintf(sendbuf,"pull,%d\n",count);
        sendto(socketid,sendbuf,strlen(sendbuf)+1,0, 
                (struct sockaddr *)&server_addr,sizeof(server_addr));
    }

    return 0;
}

