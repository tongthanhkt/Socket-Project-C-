//MY CRAPPY YOUTUBE https://www.youtube.com/channel/UC9kD0oE90HfCZvMAPzRLf3w

#include<iostream>
#include<WS2tcpip.h>
#include<thread>
#include<sstream>
#include<vector>
#pragma comment (lib, "Ws2_32.lib")
int main() {
    //INITIALIZE THE WINSOCK
    WSADATA some_kind_of_data;
    WSAStartup(MAKEWORD(2, 2), &some_kind_of_data);

    //CLIENT OBJECT
    struct CLIENT {
        char client_ip[256];
        SOCKET sock;
    };

    //CREATE A LIST OF CLIENTS
    std::vector<CLIENT> clients;

    //ACCEPT CONNECTION - THREAD 01
    std::thread accepter([&clients]() {
        ///CREATE LISTENING SOCKET
        sockaddr_in listen_address;
        listen_address.sin_family = AF_INET;
        listen_address.sin_port = htons(666);
        listen_address.sin_addr.S_un.S_addr = INADDR_ANY;
        //CREATE THE SOCKET ITSELF
        SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
        //BIND THE ADRESS TO THE SOCKET
        bind(listen_socket, (sockaddr*)&listen_address, sizeof(listen_address));
        //SET THE SOCKET TO LISTENING MODE
        listen(listen_socket, SOMAXCONN);
        //STORE CLIENT IP
        sockaddr_in client_address;
        int client_address_size = sizeof(client_address);
        //TEMPORARY CLIENT SOCKET
        SOCKET client_socket;
        //USED FOR CHANGING SOCKETS TO NON_BLOCKING
        u_long non_blocking = true;

        while (true) {
            //CREATE TEMPORARY SOCKET
            client_socket = accept(listen_socket, (sockaddr*)&client_address, &client_address_size);
            if (client_socket != INVALID_SOCKET) { //IF CLIENT CONNECTED CORRECTLLY
                //SET THE TEMPORARY SOCKET TO NON_BLOCKING
                ioctlsocket(client_socket, FIONBIO, &non_blocking);
                //CREATE NEW CLIENT OBJECT
                CLIENT new_client;
                //PUT SOCKET AND IP FROM TEMPORARY SOCKET TO NEW CLIENT OBJECT
                new_client.sock = client_socket;
                inet_ntop(AF_INET, &client_address.sin_addr, new_client.client_ip, 256);
                //ADD THE NEW CLIENT OBJECT TO THE LIST OF CLIENTS
                clients.push_back(new_client);
                //PRINT OUT THAT SOMEONE HAS CONNECTED
                std::cout << "Client " << new_client.client_ip << ":" << clients.size() - 1 << " Connected!" << std::endl;
            }
        }});

    //CHECK CONNECTIONS - THREAD 02
    std::thread checker([&clients]() {
        while (true) {
            //CYCKLE TROUGH ALL THE CLIENTS
            for (int i = 0; i < clients.size(); i++) {
                //SEND EMPTY MESSAGE TO ALL CLIENTS
                if (send(clients[i].sock, "", 1, 0) < 0) {
                    //IF CLIENT FAILS TO RECEIVE THE MESSAGE
                    //PRINT OUT MESSAGE THAT THAT CLIENT HAS DISCONNECTED
                    std::cout << "Client " << clients[i].client_ip << ":" << i << " Disconnected!" << std::endl;
                    //ERASE THE CLIENT FROM THE LIST
                    clients.erase(clients.begin() + i);
                }
            }
            //WAIT ONE SECOND BEFORE CHECKING CONNECTION AGAIN
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }});


    //RECEIVE MESSAGE - THREAD 03
    std::thread receiver([&clients]() {
        //BUFFER THAT STORES THE RECEIVED MESSAGE
        char buffer[1024];

        while (true) {
            //CYCKLE TROUGH ALL THE CLIENTS
            for (int i = 0; i < clients.size(); i++) {
                //DELETE THE OLD RECEIVED MESSAGE
                memset(buffer, 0, sizeof(buffer));

                //IF RECEIVED MESSAGE IS LONGER THEN ONE CHARACTER
                if (recv(clients[i].sock, buffer, sizeof(buffer), 0) > 1) {
                    std::cout << buffer << std::endl;
                    //CYCKLE TROUGH ALL THE CLIENTS
                    //SENDS RECEIVED MESSAGE TO ALL CONNECTED CLIENTS
                    for (int y = 0; y < clients.size(); y++)
                        //SEND MESSAGE TO ALL CLIENTS EXCEPT TO THE CLIENT THAT SEND THE MESSAGE
                        if (i != y) {
                            std::stringstream msg;
                            //CONSTRUCT THE MESSAGE
                            msg << clients[i].client_ip << ":" << i << " " << buffer;
                            //SEND THE MESSAGE
                            send(clients[y].sock, msg.str().c_str(), msg.str().size(), 0);
                        }
                }
            }
        }});

    //CONTROLL THE SERVER - MAIN THREAD
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if (msg == "stop") {
            break;
        }
    }
    WSACleanup();
    system("pause");
    quick_exit(0);
}
