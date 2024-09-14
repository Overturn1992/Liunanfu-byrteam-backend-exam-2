#ifndef NMSCLIENT_H
#define NMSCLIENT_H
#include "AuthManager.h"
#include "DataHandler.h"
#include <thread>
#include <atomic>
class NMSClient
{
public:
    NMSClient(const std::string &baseurl,const std::string &username);
    ~NMSClient();
    void run(int maxMinutes);
private:
    AuthManager authManager;
    DataHandler dataHandler;
    std::string username;
    std::thread tokenRefreshThread;
    std::atomic<bool> keepRunning;
    void retrySignup();
    void retryLogin();
    void refreshTokenLoop();
    bool submitData();
};
#endif //NMSCLIENT_H
