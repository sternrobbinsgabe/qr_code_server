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

    /*
        NOTES:
            Server messages:
                <32 bit unsigned integer representing return code>
                <32 bit unsigned integer representing URL character array length>
                <character array of size indicated>
            return codes:
                0 - Success, URL is being returned as specified below
                1 - Failure, something went wrong or no URL is being returned.
                2 - Timeout, connection closed.
                3 - Rate Limit Exceeded. An error mesage about the rate limit
                    being exceeded is set in the character array
    */

    /*
        Use ZXing library for generating and decoding QR codes.
        use jar as a decoder for QR codes.
        Invoke the decoder by typing "java -cp javase.jar:core.jar com/google/zxing.client...
        in the dir where the files are decompressed.
        In server program, you will invoce this utility using system calls
            such as system or exec
        Site any tutorial used.

    */
    /* Roadmap:
    1 ) Create a single threaded server [check]
                that accepts text froma  client [check]
                in a file [ ]
                displays the text [check]
                and disconnects the client [ ]
    2) Add support in the server for decoding QR codes [ ]
    3 ) Add support for accepting binary transmissions (change from step 1?)
    4) Support for concurrent clients
            threaded or forked [  ]
    5) Add error checking and logging func. [  ]
    6) Security features (?) [  ]

    int sockfd, new_fd, numbytes;
    struct addrinfo hints, *serverinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    //struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[128];
    char client_buf[128];

    memset(&hints, 0, sizeof hints); //set memory
    hints.ai_family=AF_UNSPEC; //do not specify family just in case
    hints.ai_socktype=SOCK_STREAM; //stream
    hints.ai_flags=AI_PASSIVE; // Can accept connections

    getaddrinfo(NULL,"12345",&hints,&serverinfo);
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    bind(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    freeaddrinfo(serverinfo);

    listen(sockfd,1); //listens to socket
    cout<<"Listening..."<<endl;
    while(1){
        sin_size=sizeof their_addr;
        new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);

        cout<<"got "<<new_fd<<endl;
        //if we dont have a connection, keep waiting
        if(new_fd==-1){
            continue;
        }
        while(1){

            numbytes=recv(new_fd,client_buf,128,0);
            //cout<<"got "<<numbytes<<" bytes"<<endl;

            if(numbytes>1){
                cout << "Got a message!" << endl;
                cout << "Type response to: "<< client_buf << endl;

                //cin to the buffer that we will send
                cin>>buf;

                inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s,sizeof s);
                cout<< "got a connection from: " << s << endl;

                //close(sockfd);
                send(new_fd,buf,128,0);
            }
        //close(new_fd);
        }
    }
    return 0;
}
