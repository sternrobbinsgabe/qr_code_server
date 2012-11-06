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
#include <limits.h>

using namespace std;


//packet structure
struct Packet
{
    public:
        long size; //size of packet
        void* data;  //payload

}; // end of packet structure

/* Returns the size of the packet
    @param Packet pointer to a packet to be sent
    @return int size
*/
int getPacketSize( Packet* packet)
{
    return sizeof (packet->size) + sizeof (packet->data);
}

/*
    Creates and returns a packet of a file.
    @param filePath - path to the file
    @return Packet struct
*/
Packet createPacket(char* filePath){

    //Packet creation:
    Packet packet = {0};

    FILE *file;
    file = fopen(filePath, "r");
    cout<<"FILE PATH: "<<filePath<<endl;

    struct stat fStat;
    stat (filePath, &fStat);
    packet.size = fStat.st_size;
    if (packet.size > 65536 || packet.size <= 0){
        cerr<<"File size invalid \n";
        exit(1);
    }

    cout<<"Size of packet: "<<packet.size<<endl;
    packet.data = malloc( packet.size );
    if ( packet.data == 0){
        cerr<<"ERROR: FILE EMPTY \n"<<endl;
        exit(1);
    }
    fread(packet.data, packet.size, 1, file);
    return packet;
}

/*
    returns -1 on failure, bytes sent on success
    @param sockfd - int socket
    @param buffer - point to buffer where the data is stored
    @bytesLeft - amount of bytes to send
*/
int sendInChunks(int sockfd, char *buffer, int bytesLeft)
{
    int bytesSent = 0;        // how many bytes we've sent
    int bytesSentSoFar = 0;

    cout<<"Bytes Left " << bytesLeft << endl;

    while(bytesLeft > 0) {
                            //socket, buffer iterator,  bytes left, flags
        bytesSentSoFar = send(sockfd, buffer+bytesSent, bytesLeft, 0);
        if (bytesSentSoFar == -1 ){
            cerr<<"ERROR: -1 bytes sent. \n"<<endl;
            exit(1);
            break;
        }
        bytesSent += bytesSentSoFar;
        bytesLeft -= bytesSentSoFar;
    }

    if (bytesSentSoFar == -1 ){
        cerr<<"ERROR: -1 bytes were sent. \n"<<endl;
        exit(1);
    }
    else {
        return bytesSent;
    }
}


///////////// END OF PACKET PROGRAMMING - LZ


//helper function to determine if we are using ipv4 or 6
void* get_in_addr(struct sockaddr* sa){
    //if ipv4, do this
    if(sa->sa_family==AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    //if not ipv4, it must be 6... I hope
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char** argv)
{
    int sockfd, numbytes;
    char buf[256];
    struct addrinfo hints, *serverinfo;
    struct their_addr;

    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints,0,sizeof hints); //set memory to safe 0
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;


    char* port = "2012"; //default port
    char* IP = "127.0.0.1"; //default IP

    //GETOPT
    //    p: -> -p  [something]
    //    i: -> -i [something_else]
    int c;
    while((c=getopt(argc,argv,"p:i:"))!=-1){
        switch(c){
        case 'p':  //port
            port=optarg;
            break;
        case 'i':
            IP=optarg;
            break;
        case '?':
            cout<< "you id something wrong" << endl;
            break;
        }
    }

    //connection setting
    rv=getaddrinfo(IP, port,&hints,&serverinfo);

    //creation of socket
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    connect(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    inet_ntop(serverinfo->ai_family,get_in_addr((struct sockaddr*)serverinfo->ai_addr),s,sizeof s);
    cout << "connecting to: "<< s << endl;

    bool b_msg_sent = false;
    char filePath[256];

    while(1){

        if(!b_msg_sent){

            cout << "Give a file path"<<endl;
            cin>>filePath;

            Packet p = createPacket(filePath);
            cout << "Sending packet "<< endl;

            int foo = sendInChunks( sockfd, (char *)p.data, p.size);
            cout<<"Sent bytes "<<foo<<endl;

            //FREE MEMORY
            free(p.data);

            //send message
            cout<<"Sending to "<< s <<endl;

            b_msg_sent = true;
            cout<<"Awaiting response..."<<endl;
        }
        else{
         numbytes=recv(sockfd,buf,256,0);

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
