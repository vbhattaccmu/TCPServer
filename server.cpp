#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
    // initialize winsock 
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cerr << "Cant initialize" << endl;
        return;
    }
    // create a socket 
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        cerr << "Cant create socket";
        return;
    }
    // bind the socket to an ip address
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));
    
    // tell winsock the socket is for listening 
    listen(listening, SOMAXCONN);

    // wait for connection 
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];
    char service[NI_MAXHOST];

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXHOST);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << "connected on port" << ntohs(client.sin_port) << endl;
    }

    // close listening socket
    closesocket(listening);
    // while loop: accept and echo message to client 
    char buf[4096];

    while (true) {
        ZeroMemory(buf, 4096);
        // wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }
        if (bytesReceived == 0) {
            cout << "Client disconnected" << endl;
            break;
        }

        // echo message to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
    // close the sock 
    closesocket(clientSocket);
    // shutdown winsock 
    WSACleanup();
}
