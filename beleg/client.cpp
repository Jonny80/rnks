//
// Created by Jan Händl on 28.01.22.
//
#include "iostream"
#include "string"
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "sys/socket.h"
#pragma comment(lib,"Ws2_32.lib")

using namespace std;
struct message {
    string name;
    char* data;
} _message;
int main(int argc, char *argv[]){

    string name = "s82807_Client";
    string address = "::1";
    int port = 50000;

    if (argc != 3) {
        cout << "Bitte [Port] [IP] [Name] als Programmargumente übergeben" << endl;
        cout << "Nutze default Werte" << endl;
    } else  {
        port = (int)*argv[0];
        if (port < 49151 || port > 65535){
            cout << "Üngültiger Port bitte Port zwischen 49151 und 65535 benutzen" << endl;
            cout << "Benutze default Port: 50000 " << endl;
            port = 50000;
        }
        address = argv[1];
        name = argv[2];
    }
    cout << "Wilkommen " << name << endl;


    cout << "Erstelle Socket" << endl;



    int clientSock = socket(AF_INET6,SOCK_STREAM,0);

    if (clientSock == -1) {
        cerr << "Socket konnte nicht erstellt werden" << endl;
        return -1;
    }
    cout << "Socket erstellt" << endl;


    sockaddr_in6 information;
    information.sin6_family = AF_INET6;
    information.sin6_port = htons(port);
    inet_pton(AF_INET6,address.c_str(),&information.sin6_addr);


    int Result = connect(clientSock,(sockaddr*)&information,sizeof(information));

    if (Result == -1) {
        cout << "Client konnte keine Serverinformation sammeln" << endl;
        return -1;
    }else {
        cout << "Mit dem Server verbunden" << endl;
    }
    _message.name = name;
    char buffer[1024];
    string message;
    _message.data = buffer;
    while(true) {
        cout << _message.name<< "> ";
        getline(cin,message);
        if (message == "exit()") {
            cout << "Beende Programm" << endl;
            break;
        }
        if (!message.empty()) {
            int sendMessage = send(clientSock,message.c_str(),message.size() + 1,0);
            if (sendMessage == -1) {
                cout << "Client konnt keine Nachricht senden" << endl;
                continue;
            }
            memset(buffer,0,1024);
            int receivedMessage = recv(clientSock,buffer,1024,0);
            if (receivedMessage > 0) {
                cout << "Server: " << string(buffer,0,receivedMessage) << endl;
            }
        }
        }

    close(clientSock);
    return 0;
}
