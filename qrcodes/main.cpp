//THIS IS THE SERVER


/**
 @author Gabriel Stern-Robbins, Lukasz Zawada
 @copyright 10/27/2012 WPI CS3516 B12
 @comment: This is the server for assignment 1.

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
#include <time.h>
#include <sys/stat.h>

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

//function that gets the url from a qr file
char* qr2url(char* fileName){
    char temp[256]={0};
    strcpy(temp,"java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner ");
    strcat(temp, fileName);
    FILE* fp;
    fp = popen(temp,"r");
    char* line= (char*)malloc(1024 * sizeof(char));
    size_t len=1024;
    while((getline(&line,&len,fp)) > 0){
        if(strcmp(line,"Parsed result:\n")==0){
            getline(&line,&len,fp);
            pclose(fp);
            return line;
        }
    }
    pclose(fp);
    return "you dun goofed\n";
}


int sockfd, new_fd, numbytes;

void init(char* p){

    int yes=1;
    struct addrinfo hints, *serverinfo;

    memset(&hints, 0, sizeof hints); //set all the members
    hints.ai_family=AF_UNSPEC; //do not specify family just in case
    hints.ai_socktype=SOCK_STREAM; //stream
    hints.ai_flags=AI_PASSIVE; // Can accept connections

    getaddrinfo(NULL,p,&hints,&serverinfo);
    sockfd=socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    bind(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);

    freeaddrinfo(serverinfo);

    listen(sockfd,1); //listens to socket
}

int main(int argc, char** argv){
    //config variables that hold the commandline args
    //-p 12345 -q 6 -r 30 -m 5 -t 60
    char* port="2012";//p
    int rateReq=3;//q
    int rateTime=60;//r
    int maxUsers=3;//m
    int timeOut=80;//t

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
            cout<< "you did something wrong" << endl;
            break;
        }

    }

    //setup the internets
    init(port);

    //for the forking
    int status;

    //open the log file
    FILE* log=fopen("log.txt","a");
    //get the time of day
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    fprintf(log,"Log file started for %d-%d-%d %d:%d:%d \n",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    fflush(log);
    //fclose(log);

    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    char s[INET6_ADDRSTRLEN];
    char buf[65536]={0};
    char client_buf[65536]={0};
    int numUsers=0;

    cout<<"Listening..."<<endl;
    while(1){
        sin_size=sizeof their_addr;
        new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);

        cout<<"got "<<new_fd<<endl;
        //if we dont have a connection, keep waiting
        if(new_fd==-1){
            continue;
        }
        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s,sizeof s);
        time(&rawtime);
        timeinfo=localtime(&rawtime);
        fprintf(log,"%d-%d-%d %d:%d:%d Connection from: %s\n",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,s);
        fflush(log);
        numUsers++;
        cout<< "num users " << numUsers << endl;
        pid_t pID = fork();

        if(pID==0){ //child
            cout<<"This is child of pID: "<<pID<<endl;
            //Create a new file for output
            FILE *filet = fopen("client_qr.png", "w+");
            while(1){

                numbytes=recv(new_fd,client_buf,65536,0);

                if(numbytes>0){
                    cout << "Got a message!" << endl;

                    //write into the file
                    for ( int i = 0; i < numbytes; i++){
                        fputc(client_buf[i],filet);
                    }
                }
                else{
                    continue;
                }
                fclose(filet);
                break;
            }
            cout<<"Decoding QR"<<endl;
            //QR DECODING HERE
            inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s,sizeof s);

            int returnCode = 0;

            char* url = qr2url("client_qr.png");

            //GET RETURN CODE HERE
            if ( &url[0] != "-" && &url[1] != "1"){
                returnCode = 0; //0 = Success
            }
            else{
                returnCode = 1;
            }


            char message2Client[65536] = {0};
            char retCode = (char)(((int)'0')+returnCode);
            char *newCode = &retCode;

            strcpy( message2Client, newCode );

            switch ( returnCode ){
                case 0:
                    strcat( message2Client, " - Success \n ");
                    strcat( message2Client, url);
                    break;
                case 1:
                    strcat( message2Client, " - Failure \n ");
                    strcat( message2Client, "No URL or something went wrong.");
                    break;
                case 2:
                    strcat( message2Client, " - Timeout \n ");
                    break;
                case 3:
                    strcat( message2Client, " - Rate Limit Exceeded \n ");
                    break;
                default:
                    break;
            }

            send(new_fd,message2Client,128,0);

            //cout<<qr2url("client_qr.png");
            //send(new_fd,qr2url("client_qr.png"),128,0);
            time(&rawtime);
            timeinfo=localtime(&rawtime);
            fprintf(log,"%d-%d-%d %d:%d:%d Decoded a qr code with url: %s\n",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,qr2url("client_qr.png"));
            fflush(log);
            remove("client_qr.png");
            cout<<"Finished sending packet."<<endl;
            close(new_fd);
            exit(0);
        }
        else if (pID < 0){ //fork failed
            //CODE FROM SITE:
            cout<<"Failed to fork"<<endl;
            exit(1);
        }
        else{ //parent
            //if no more than 3 people
            cout<<"This is parent"<<endl;
            new_fd = -1;
            //listen(sockfd,1); //listens to socket
            //wait for child

            //while(!waitpid(pID,&status,WNOHANG)){}
            //cout << "done waiting" << endl;
        }
    }
    return 0;
}
