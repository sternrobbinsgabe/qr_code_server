//THIS IS THE SERVER


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
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

using namespace std;
//recieve a string from the client and then print it

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
    //to do
    //create a server on port 12345
    //listen for incomming messages
    //if it recieves one, then convert string to ALL CAPS and send it back to the client
    //if it recieves an escape, close the connection

    int sockfd, new_fd;
    struct addrinfo hints, *serverinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    //struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[128];

    memset(&hints, 0, sizeof hints); //set memory
    hints.ai_family=AF_UNSPEC; //do not specify family just in case
    hints.ai_socktype=SOCK_STREAM; //stream
    hints.ai_flags=AI_PASSIVE; // Can accept connections

    getaddrinfo(NULL,"12345",&hints,&serverinfo);
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    bind(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    freeaddrinfo(serverinfo);

    listen(sockfd,1); //listens to socket

    while(1){
        sin_size=sizeof their_addr;
        new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);

        //if we dont have a connection, keep waiting
        if(new_fd==-1){
            continue;
        }
        //cin to the buffer that we will send
        while(1){

            cout << "Type something" << endl;
            cin>>buf;

            inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s,sizeof s);
            cout<< "got a connection from: " << s << endl;

        //close(sockfd);
                send(new_fd,buf,128,0);
        }
        //close(new_fd);
    }

    return 0;
}

