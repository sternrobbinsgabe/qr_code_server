//THIS IS THE CLIENT
/**
 @author Gabriel Stern-Robbins, Lukasz Zawada
 @copyright 10/27/2012 WPI CS3516 B12
 @comment: This is the client for assignment 1.

**/

#include <iostream>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

using namespace std;


//packet structure
struct Packet
{
    public:
        long size; //size of packet
        void* data;  //payload

}; // end of packet structure

int getPacketSize( Packet* packet)
{
    return sizeof (packet->size) + sizeof (packet->data);
}

Packet createPacket(char* filePath){

    //Packet creation:

    Packet packet = {0};

    FILE *file;
    file = fopen(filePath, "r");
    cout<<"FILEPATH: "<<filePath<<endl;

    //packet.size = getFileSize (file);
    struct stat fStat;
    stat (filePath, &fStat);
    packet.size = fStat.st_size;

    cout<<"Size of packet: "<<packet.size<<endl;
    packet.data = malloc( packet.size);
    if ( packet.data == 0)
        cout<<"ERROR: FILE EMPTY"<<endl;

    cout<<"Size of packet data: "<<packet.data<<endl;
    int n = fread(packet.data, packet.size, 1, file);
    cout<<"Whatever n is: "<<n<<endl;
    return packet;
}

//returns -1 on failure, bytesSent on success
int sendInChunks(int sockfd, char *buffer, int bytesLeft)
{
    int bytesSent = 0;        // how many bytes we've sent
    int bytesSentSoFar = 0;

    cout<<"socket/bytesLeft "<< sockfd << " / " << bytesLeft << endl;

    while(bytesLeft > 0) {
                            //socket, buffer iterator,  bytes left, flags
        bytesSentSoFar = send(sockfd, buffer+bytesSent, bytesLeft, 0);
        if (bytesSentSoFar == -1 ){
            cout<<"ERROR 001: -1 bytes sent."<<endl;
            break;
        }
        bytesSent += bytesSentSoFar;
        bytesLeft -= bytesSentSoFar;
    }

    if (bytesSentSoFar == -1 ){
        cout<<"ERROR 002: -1 bytes were sent."<<endl;
        return -1;
    }
    else {
        return bytesSent;
    }
}


///////////// END OF PACKET

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
    char filePath[256];

    while(1){

        if(!b_msg_sent){
            cout<<"Give a file path"<<endl;
            cin>>filePath;

            cout<<filePath<<endl;


            Packet p = createPacket(filePath);

            int foo = sendInChunks( sockfd, (char *)p.data, p.size);
            cout<<"sent bits"<<foo<<endl;
            //send(sockfd,filePath,128,0);
            free(p.data);
            //send message
            //cout<<"Attempting to send message: "<<str_msg_buf<<endl;
            cout<<"Sending to "<< s <<endl;

            //int foo = send(sockfd,str_msg_buf,128,0);
            //cout<<"sent bits"<<foo<<endl;

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
