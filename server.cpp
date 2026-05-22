#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h> // 需要加入這個來支援現代網路位址轉換

// 告訴編譯器要連結 Windows 網路函式庫
#pragma comment(lib, "ws2_32.lib") 

using namespace std;

class MultiThreadServer {
private:
    SOCKET server_fd; // Windows 中，Socket 的型別是 SOCKET
    struct sockaddr_in address;

    void handleClient(SOCKET client_socket) {
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        
        if (recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
            cout << "[執行緒 " << this_thread::get_id() << "] 收到訊息: " << buffer;
            string reply = "伺服器已收到並處理完畢。\n";
            send(client_socket, reply.c_str(), reply.length(), 0);
        }
        
        closesocket(client_socket); // Windows 使用 closesocket 關閉連線
    }

public:
    MultiThreadServer(int port) {
        // Windows 特有的步驟：初始化 Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Winsock 初始化失敗！" << endl;
            exit(1);
        }

        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == INVALID_SOCKET) {
            cerr << "建立 Socket 失敗！" << endl;
            exit(1);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
            cerr << "綁定 Port 失敗！" << endl;
            exit(1);
        }

        listen(server_fd, 10);
        cout << "伺服器啟動，監聽 Port " << port << "..." << endl;
    }

    ~MultiThreadServer() {
        if (server_fd != INVALID_SOCKET) closesocket(server_fd);
        // 清理 Winsock 資源
        WSACleanup();
    }

    void start() {
        while (true) {
            int addrlen = sizeof(address);
            // Windows 中，accept 參數型別略有不同
            SOCKET client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
            if (client_socket == INVALID_SOCKET) continue;

            thread clientThread(&MultiThreadServer::handleClient, this, client_socket);
            clientThread.detach(); 
        }
    }
};

int main() {
    MultiThreadServer server(8080);
    server.start();
    return 0;
}
