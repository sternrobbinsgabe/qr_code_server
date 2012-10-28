//THIS IS THE CLIENT
/**
 @author Gabriel Stern-Robbins, Lukasz Zawada
 @copyright 10/27/2012 WPI CS3516 B12
 @comment: This is the client for assignment 1.

**/

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;


//for the checkpoint, the client will cin to a string and then the server will print that string

//helper function to determine if we are using ipv4 or 6
void* get_in_addr(struct sockaddr* sa){
    //if ipv4, do this
    if(sa->sa_family==AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    //if not ipv4, it must be 6... I hope
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
    //connect to server on 12345
    //listen for messages from the server
    //then print them
    int sockfd, numbytes;
    char buf[128];
    struct addrinfo hints, *serverinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;

    //connection setting
    rv=getaddrinfo("cs3516machine","12345",&hints,&serverinfo);

    //creation of socket
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    connect(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    inet_ntop(serverinfo->ai_family,get_in_addr((struct sockaddr*)serverinfo->ai_addr),s,sizeof s);
    cout << "connecting to: "<< s << endl;

    while(1){
        numbytes=recv(sockfd,buf,127,0);
        //If the number of bytes received is more than 1
        //then the message was gotten
        if(numbytes>1){
            buf[numbytes]='\0';
            cout<< "Recieved " <<numbytes<< " bytes"<< endl;
            cout << "Recieved "<< buf << endl;
        }
    }


    return 0;
}
