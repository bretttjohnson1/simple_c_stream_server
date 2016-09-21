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
#include <string.h>

void fail(char *err){
   printf("%s Error %s \n",err,strerror(errno));
   exit(1);
}
int main(int argc, char **argv){
   if(argc !=2){
      printf("missing args\nRun ./server -h for help\n");
      exit(1);
   }
   char *filename = argv[1];
   if(strcmp(argv[1],"-h")==0){
      printf("server usage\n./server <filename>\n\nTo stream an mp3 file run the server as ./server file.mp3\nthen run the client as ./client <server_adress> | mpg123 -\n");
      exit(0);
   }
   int status;
   struct addrinfo hints;
   struct addrinfo *serverinfo;
   memset(&hints,0,sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   status = getaddrinfo(NULL, "8080", &hints,&serverinfo);
   if(status!=0){
      printf("err %s \n",strerror(errno));
      exit(1);
   }
   int sockfd = socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);

   int yes=1;
   if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
       perror("setsockopt");
       exit(1);
   }
   bind(sockfd, serverinfo->ai_addr,serverinfo->ai_addrlen);
   listen(sockfd, 5);
   int newfd;
   struct sockaddr_storage their_addr;
   socklen_t addr_size;
   addr_size = sizeof(their_addr);

   FILE *f = fopen(filename,"rb");
   char *filebuffer;
   long filelength = 0;
   fseek(f,0,SEEK_END);
   filelength = ftell(f);

   rewind(f);
   filebuffer = (char *)malloc((filelength+1)*sizeof(char));
   long var=0;
   for(var =0;var<filelength;var++){
         *(filebuffer+var)=getc(f);
   }
   *(filebuffer+filelength)='\0';
   fclose(f);

   printf("%ld\n",filelength );
   fflush(stdout);
   int success;
   while(1){
      printf("waiting...\n");
      fflush(stdout);
      while(1){
         newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
         if(newfd == -1)
            close(newfd);
         else{

            success = send(newfd, &filelength,4,0);
            if(success == -1){
               fail("send length");
            }
            break;
         }

      }
      long index = 0;

      while(index<filelength){
         success = send(newfd, &filebuffer[index],1,0);
         if(success == -1){
            printf("Transmission closed at %ld\n bytes",index );
            //fail("send data");
            break;
         }
         index+=1;
      }
      printf("sent data\n");
      fflush(stdout);
   }

   free(filebuffer);
   close(newfd);
   close(sockfd);
   exit(0);
}
