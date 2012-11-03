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
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

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

//returns -1 on failure, bytesSent on success
int sendInChunks(int sockfd, char *buffer, int bytesLeft)
{
    int bytesSent = 0;        // how many bytes we've sent
    int bytesSentSoFar;

    while(bytesLeft > 0) {
                            //socket, buffer iterator,  bytes left, flags
        bytesSentSoFar = send(sockfd, buffer+bytesSent, bytesLeft, 0);
        if (bytesSentSoFar == -1 ){
            cout<<"ERROR: -1 bytes sent."<<endl;
            break;
        }
        bytesSent += bytesSentSoFar;
        bytesLeft -= bytesSentSoFar;
    }

    if (bytesSentSoFar == -1 ){
        cout<<"ERROR: -1 bytes were sent."<<endl;
        return -1;
    }
    else {
        return bytesSent;
    }
}

//function that gets the url from a qr file
char* qr2url(char* fileName){
    char temp[256]={0};
    strcpy(temp,"java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner ");
    strcat(temp,fileName);
    FILE* fp=popen(temp,"r");
    char* line=NULL;
    size_t len=0;
    ssize_t read;

    while((read=getline(&line,&len,fp))!=-1){
        //cout << line;
        if(strcmp(line,"Parsed result:\n")==0){
            getline(&line,&len,fp);
            return line;
            break;
        }
    }
    pclose(fp);
    return NULL;
}


int sockfd, new_fd, numbytes;

void init(char* p){

    struct addrinfo hints, *serverinfo;

    memset(&hints, 0, sizeof hints); //set all the members
    hints.ai_family=AF_UNSPEC; //do not specify family just in case
    hints.ai_socktype=SOCK_STREAM; //stream
    hints.ai_flags=AI_PASSIVE; // Can accept connections

    getaddrinfo(NULL,p,&hints,&serverinfo);
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    bind(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    freeaddrinfo(serverinfo);

    listen(sockfd,1); //listens to socket
}

int main(int argc, char** argv)
{


    //config variables that hold the commandline args
    //-p 12345 -q 6 -r 30 -m 5 -t 60
    char* port="2012";//p
    int rateReq=3;//q
    int rateTime=60;//r
    int maxUsers=3;//m
    int timeOut=80;//t

    cout<< port << endl;
    cout << rateReq << endl;
    cout << rateTime << endl;
    cout << maxUsers << endl;
    cout << timeOut << endl;

    //do getopt funtime
    int c;
    while((c=getopt(argc,argv,"p:q:r:m:t:"))!=-1){
        switch(c){
        case 'p':
            port=optarg;
            break;
        case 'q':
            rateReq=atoi(optarg);
            break;
        case 'r':
            rateTime=atoi(optarg);
            break;
        case 'm':
            maxUsers=atoi(optarg);
            break;
        case 't':
            timeOut=atoi(optarg);
            break;
        case '?':
            cout<< "you id something wrong" << endl;
            break;
        }

    }

    cout << port << endl;
    cout << rateReq << endl;
    cout << rateTime << endl;
    cout << maxUsers << endl;
    cout << timeOut << endl;

    //setup the internets
    init(port);

    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    char s[INET6_ADDRSTRLEN];
    char buf[128];
    char client_buf[128];

    cout<<"Listening..."<<endl;
    while(1){
        sin_size=sizeof their_addr;
        new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);

        cout<<"got "<<new_fd<<endl;
        //if we dont have a connection, keep waiting
        if(new_fd==-1){
            continue;
        }

	pid_t pID = fork();

	if(pID == 0) //child
	{

       	    cout<<"This is child of pID: "<<pID<<endl;;
            //Create a new file for output
            FILE *filet = fopen("client_qr.png", "w+");

        while(1){

            numbytes=recv(new_fd,client_buf,2000,0);

            if(numbytes>1){
                cout << "Got a message!" << endl;
                cout << "Type response to: "<< client_buf << endl;

		//write into the file
                for ( int i = 0; i < 1589; i++){
                    fputc(client_buf[i],filet);
                }
            }
            break;
        }
        cout<<"Decoding QR"<<endl;
        //QR DECODING HERE
        char *new_buf;
        new_buf=qr2url("client_qr.png");
        //cout<<new_buf<<endl;
        /*for(int i = 0; i < 128; i++){
            if(buf[i] > -1){
                buf[i] = new_buf[i];
                cout<<buf[i];
            }
        }*/
        cout<<"Finished translating to char array"<<endl;

        cout<<"test4"<<endl;
        cout<<qr2url("qr.png");
        cout<<"test3"<<endl;

        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s,sizeof s);
        cout<<"test1"<<endl;
        cout<<qr2url("qr.png");
        cout<<"test2"<<endl;

        send(new_fd,qr2url("qr.png"),128,0);
        cout<<"Finished sending packet."<<endl;

        close(new_fd);
        }
        else if (pID < 0) //fork failed
        {
            //CODE FROM SITE:
            //cerr<<"Failed to fork"<<endl;
            //exit(1);
            cout<<"Error: Failed to create server child."<<endl;

        }
        else //parent
        {
            //if no more than 3 people
            cout<<"This is parent"<<endl;
            new_fd = -1;
            listen(sockfd,1); //listens to socket
            continue;
        }
    }
    return 0;
}
