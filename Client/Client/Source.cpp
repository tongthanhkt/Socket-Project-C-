#include<iostream>
#include<WS2tcpip.h>
#include<sstream>
#include<thread>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
int main() {
    //INITIALIZE WINSOCK
    WSADATA some_kind_of_data;
    WSAStartup(MAKEWORD(2, 2), &some_kind_of_data);

    //CREATE CONNECTION SOCKET
    sockaddr_in connect_adress;
    connect_adress.sin_family = AF_INET;
    connect_adress.sin_port = htons(666);
    inet_pton(AF_INET, "127.0.0.1", &connect_adress.sin_addr);
    SOCKET connection_socket = socket(AF_INET, SOCK_STREAM, 0);

    //CONNECT TO SERVER - THREAD 01
    bool is_connected = false;

    std::thread connector([&connection_socket, &connect_adress, &is_connected]() {
        while (true) {
            //TRY SENDING MESSAGE TO SEE IF ITS CONNECTED
            if (send(connection_socket, "", 1, 0) == SOCKET_ERROR) {
                //IF NOT CONNECTED :
                is_connected = false;
                //RESET THE CONNECTION SOCKET
                connection_socket = socket(AF_INET, SOCK_STREAM, 0);
                //TRY TO CONNECT TO THE SERVER, IN A LOOP
                while (true) {
                    std::cout << "Trying to connect to server...\n";
                    if (connect(connection_socket, (sockaddr*)&connect_adress, sizeof(connect_adress)) != SOCKET_ERROR) {
                        //IF IT CONNECTS TO THE SERVER
                        std::cout << "Connected to server!\n";
                        is_connected = true;
                        //GOES BACK TO SENDING MESSAGES TO SEE IF ITS STILL CONNECTED
                        break;
                    }
                }
            }
            //SLEEP FOR ONE SECOND
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }});

    //RECEIVE MESSAGES FROM SERVER - THREAD 02
    std::thread receiver([&connection_socket, &is_connected]() {
        //STORE RECEIVED MESSAGE INSIDE THIS BUFFER
        char buffer[1024];
        while (true) {
            if (is_connected == true) {
                //RESET BUFFER EVERY TIME
                memset(buffer, 0, sizeof(buffer));
                //IF MESSAGE RECEIVED IS LONGER THEN ONE BYTE, PRINT IT OUT
                if (recv(connection_socket, buffer, sizeof(buffer), 0) > 1)
                    std::cout << buffer << std::endl;
            }
            //SLEEP FOR 1S IF NOT CONNECTED
            else std::this_thread::sleep_for(std::chrono::seconds(1));
        }});

    //SEND A MESSAGES - MAIN THREAD
    cout << "Nhap lua chon " << endl;
    cout << "1. Dang nhap " << endl;
    cout << "2. Dang ki " << endl;
    int n;
    cin >> n;
    cin.ignore();
    if (n == 2) {
        
        

        while (true) {
           string username;
           string password;
            if (is_connected == true) {
                //ASK USER TO TYPE THE MESSAGE
                
                cout << "Entern username ";
                getline(cin, username);
                //SEND THE MESSAGE
                if (send(connection_socket, username.c_str(), username.size() + 1, 0) <= 0)
                    //IF IT FAILS TO SEND THE MESSAGE
                    std::cout << "Failed to send the message...\n";
               
                
                cout << "Entern password ";
                getline(cin, password);
                //SEND THE MESSAGE
                if (send(connection_socket, password.c_str(), password.size() + 1, 0) <= 0)
                    //IF IT FAILS TO SEND THE MESSAGE
                    std::cout << "Failed to send the message...\n";
            }
            //SLEEP FOR 1S IF SOCKET IS NOT CONNECTED
            else std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        closesocket(connection_socket);
        WSACleanup();
        quick_exit(0);
    }
    
    
    
    
    
}
