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
    int sockfd, numbytes, new_fd;
    char buf[128];
    struct addrinfo hints, *serverinfo, *p;
    struct their_addr;
        socklen_t sin_size;

    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;

    //connection setting
    rv=getaddrinfo("127.0.0.1","12345",&hints,&serverinfo);

    //creation of socket
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    connect(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    inet_ntop(serverinfo->ai_family,get_in_addr((struct sockaddr*)serverinfo->ai_addr),s,sizeof s);
    cout << "connecting to: "<< s << endl;

    bool b_msg_sent = false;
    //msg buffer
    char str_msg_buf[128];


    while(1){

        if(!b_msg_sent){
            cout<<"Send a message to server"<<endl;
            cin>>str_msg_buf;
            //send message
            cout<<"Attempting to send message: "<<str_msg_buf<<endl;
            cout<<"Sending to "<< s <<endl;
            int foo = send(sockfd,str_msg_buf,128,0);
            cout<<"sent bits"<<foo<<endl;

            b_msg_sent = true;
            cout<<"Awaiting response..."<<endl;
        }
        else{
         numbytes=recv(sockfd,buf,128,0);

            if(numbytes>1){
                    //If the number of bytes received is more than 1
                    //then the message was gotten

                buf[numbytes]='\0';
                cout<< "Recieved " <<numbytes<< " bytes"<< endl;
                cout << "Recieved "<< buf << endl;
                b_msg_sent = false;
            }
        }
    }


    return 0;
}
