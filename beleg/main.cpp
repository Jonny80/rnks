//
// Created by Jan Händl on 28.01.22.
//
#include "iostream"
#include "string"
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "sys/socket.h"
#pragma comment(lib,"Ws2_32.lib")

using namespace std;
struct message {
    string name;
    char* data;
} _message;

int main(int argc, char *argv[]){
    string name = "s82807_Server";
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
    // socket initializing
    int serverSocket = socket(AF_INET6,SOCK_STREAM,0);
    sockaddr_in6 information;

    if (serverSocket == -1){
        cerr << "Socket konnte nicht erstellt werden" << endl;
        return -1;
    }

    cout << "Socket erstellt" << endl;

    information.sin6_family = AF_INET6;
    //information.sin_addr = address.c_str();
    information.sin6_port = htons(port);
    // returns 1 for valid address
    if(inet_pton(AF_INET6,address.c_str(),&information.sin6_addr)!=1) {
        cout << "Ungültige Ip Addresse: Benutze default Addresse: ::1" << endl;
        cout << inet_pton(AF_INET6,"::1",&information.sin6_addr) << endl;
    }

    //bind
    cout << "Binde Socket an . . . " << endl;
    if (::bind(serverSocket,(struct sockaddr *)&information,sizeof(information)) == -1){
            cerr << "Binden des Sockets fehlgeschlagen" << endl;
            return -1;
    }
    cout << "Binden des Sockets erfolgreich" << endl;

    if (listen(serverSocket,SOMAXCONN) == -1) {
        cerr << "Socket kann nicht auf Verbindung hören" << endl;
        return -1;
    }

    cout << "Listening . . . " <<endl;

    sockaddr_in6 clientInformation;
    socklen_t clientLen = sizeof(clientInformation);


    //accept
    int clientSocket = ::accept(serverSocket,(struct sockaddr*) &clientInformation,&clientLen);

    if (clientSocket == -1) {
        cerr << "Client kann sich nicht verbinden" << endl;
        return -1;
    }

    close(serverSocket);

    struct addrinfo config;
    struct addrinfo *result;

    memset(&config,0,sizeof(config));

    if(getaddrinfo("::1" ,"50000" ,&config,&result) != 0)
    {
        cout << "Konnte keine Clientinformationen erhalten" << endl;
        return -1;
    } else{
        cout << "Mit dem Client verbunden" << endl;
    }

    _message.name = name;

    char buffer[1024];
    _message.data = buffer;
    string message;
    while(true) {
        cout << _message.name<< "> ";
        getline(cin,message);
        if (message == "exit()") {
            cout << "Beende Programm" << endl;
            break;
        }
        if (!message.empty()) {
            int sendMessage = send(clientSocket,message.c_str(),message.size() + 1,0);
            if (sendMessage == -1) {
                cout << "Client konnt keine Nachricht senden" << endl;
                continue;
            }
            memset(buffer,0,1024);
            int receivedMessage = recv(clientSocket,buffer,1024,0);
            if (receivedMessage > 0) {
                cout << "Server: " << string(buffer,0,receivedMessage) << endl;

            }
        }
         }
    close(clientSocket);
    return 0;
}