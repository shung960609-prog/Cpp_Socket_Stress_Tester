#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <atomic> // 加入原子操作，用於精準計數

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// 宣告兩個全域的計數器
atomic<int> success_count(0);
atomic<int> fail_count(0);

class SimpleClient {
// ... (這裡的 SimpleClient 類別內容完全不變，跟之前一樣) ...
private:
    SOCKET sock;
    struct sockaddr_in server;
    bool wsaInitialized;

public:
    SimpleClient(string ip, int port) {
        WSADATA wsaData;
        wsaInitialized = (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_addr.s_addr = inet_addr(ip.c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
    }
    ~SimpleClient() {
        if (sock != INVALID_SOCKET) closesocket(sock);
        if (wsaInitialized) WSACleanup();
    }
    bool connectToServer() {
        return connect(sock, (struct sockaddr *)&server, sizeof(server)) != SOCKET_ERROR;
    }
    void sendData(string data) {
        send(sock, data.c_str(), data.length(), 0);
    }
};

void attack(int id) {
    SimpleClient client("127.0.0.1", 8080);
    
    if (client.connectToServer()) {
        string request = "[攻擊封包] 我是打手 #" + to_string(id) + "\n";
        client.sendData(request);
        success_count++; // 攻擊成功，計數器 +1
    } else {
        fail_count++;    // 攻擊失敗（被伺服器拒絕），計數器 +1
    }
}

int main() {
    int THREAD_COUNT = 5000; // 保持 5000
    vector<thread> attackers;

    cout << "準備發動壓力測試，瞬間產生 " << THREAD_COUNT << " 個連線..." << endl;

    for (int i = 0; i < THREAD_COUNT; i++) {
        attackers.push_back(thread(attack, i));
    }

    cout << "等待攻擊結束..." << endl;
    for (auto& t : attackers) {
        if (t.joinable()) t.join();
    }

    // 印出最終戰損報告
    cout << "\n========== Attack Report ==========" << endl;
    cout << "Total Connections: " << THREAD_COUNT << endl;
    cout << "Success (Connected): " << success_count << endl;
    cout << "Failed (Refused): " << fail_count << endl;
    cout << "===================================" << endl;

    system("pause");
    return 0;
}
