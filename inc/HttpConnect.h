#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>

#include "Utils.h"
class HttpConnect {
public:
    HttpConnect();
    virtual ~HttpConnect();

    std::string PostSendJSONToAIServerAndRetContent(
        const char *host, int port,
        const char *path, char *fBuff, int fSize);
    std::string PostSendJSONToAIServerAndRetContent(
        const std::string host, int port,
        const std::string path, const std::string json_str);
private:
    void SocketHttp(
        const std::string host, int port, std::string buff, int len, int type);

};




// class Base {
// public:
//     static void Myusleep(int microseconds) {
//         std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
//     }
// };

// class TCPClient {
//     std::string host;
//     int port;
//     int connectStatus = 0; // 0 means not connected, 1 means connected
//     std::thread *recvThread = nullptr;
//     void (Base::*recvCallback)(int) = nullptr;
//     Base *recvCallbackObject = nullptr;

// public:
//     TCPClient(std::string host, int port) : host(host), port(port) {}

//     ~TCPClient() {
//         if (recvThread) {
//             recvThread->join();
//             delete recvThread;
//         }
//     }

//     void ConnectToServer() {
//         // Implementation of connecting to server goes here.
//         // After successfully connecting, set connectStatus to 1.
//     }

//     void SendMsgBySocket(const std::string &msg, int len) {
//         // Implementation of sending message goes here.
//     }

//     int GetConnectStatus() {
//         return connectStatus;
//     }

//     void SetRecvCallFun(int param, Base *object, void (Base::*func)(int)) {
//         recvCallback = func;
//         recvCallbackObject = object;
//     }

//     void RunRecvThread() {
//         // Creating a new thread that will run the function "ReceiveData".
//         recvThread = new std::thread(&TCPClient::ReceiveData, this);
//     }

// private:
//     void ReceiveData() {
//         // Implementation of receiving data goes here.
//         // After receiving data, if recvCallback is not NULL, call it.
//         if (recvCallback && recvCallbackObject) {
//             (recvCallbackObject->*recvCallback)(1); // Assume 1 is the received data.
//         }
//     }
// };