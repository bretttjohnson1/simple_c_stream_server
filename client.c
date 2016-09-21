#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

void fail(){
   printf("err %s \n",strerror(errno));
   exit(1);
}

int main(int argc, char **argv){
   if(argc !=2){
      printf("missing args\nRun ./client -h for help\n");
      exit(1);
   }
   char *address = argv[1];
   /*struct sockaddr_in6 sa;
   char ip6[INET_ADDRSTRLEN];
   inet_ntop(AF_INET6,&(sa.sin6_addr),ip6, INET6_ADDRSTRLEN);
   printf("%s\n", ip6);*/
   if(strcmp(argv[1],"-h")==0){
      printf("client usage\n./client <server_adress>\n\nTo stream an mp3 file run the server as ./server file.mp3\nthen run the client as ./client <server_adress> | mpg123 -\n");
      exit(0);
   }

   int status;
   struct addrinfo hints;
   struct addrinfo *serverinfo;
   memset(&hints,0,sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   status = getaddrinfo(address, "8080", &hints,&serverinfo);

   int sockfd = socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
   //bind(sockfd, serverinfo->ai_addr,serverinfo->ai_addrlen);
   int success = connect(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);
   if(success!=0){
      fail();
   }

   long index = 0;
   long size = 0;
   success = recv(sockfd,&size,4,0);
   if(success==-1){
      fail();
   }
   //printf("%ld\n", size);
   char buff[1];
   while (index<size) {
      success = recv(sockfd,buff,1,0);
      if(success==-1){
         fail();
      }
      fwrite(buff,1,1,stdout);
      fflush(stdout);
      index++;
   }

}
