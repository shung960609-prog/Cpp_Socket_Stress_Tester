#  C++ Multi-threaded Socket & Stress Test Simulation

這是一個基於 C++11 、 Windows Winsock2 API 實作的主從式架構、本機壓力測試(DDoS 模擬)。

主要透過實作底層網路Socket通訊，探討伺服器在面對高併發連線時的資源分配、耗盡現象（如 File Descriptor 限制與連線拒絕）。

## 核心功能與技術特色

* **多執行緒伺服器：** 突破單執行緒的阻塞限制，主執行緒專職 `accept` 監聽，並動態分配 `std::thread` 處理每一筆獨立連線。
* **高併發壓力測試：** 瞬間釋放數千個執行緒向伺服器發起 TCP 連線，模擬分散式阻斷服務攻擊 (DDoS) 的流量特徵。
* **原子操作戰損統計：** 運用 `<atomic>` 確保在極端多執行緒環境下的計數精準度，即時回報成功突圍與被伺服器拒絕的連線數量。
* **Windows API Integration：** 完整處理 Windows 環境下的 Winsock 初始化 (`WSAStartup`) 與資源釋放機制。

## 開發環境、依賴清單

* **作業系統：** Windows 
* **編譯器：** GCC (MinGW-w64) / 支援 C++11 標準
* **依賴函式庫：** `ws2_32.lib` (Windows Sockets 2)

## 快速啟動

請開啟兩個命令提示字元 (終端機) 視窗，並確保編譯器支援 `-std=c++11` 與 `-pthread`。

### 1. 編譯專案
```bash
# 編譯伺服器端
g++ server.cpp -o server -std=c++11 -pthread -lws2_32

# 編譯壓力測試端
g++ stress_test.cpp -o stress_test -std=c++11 -pthread -lws2_32
