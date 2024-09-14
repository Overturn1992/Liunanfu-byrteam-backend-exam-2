#include "NMSClient.h"
#include <spdlog/spdlog.h>
#include <iostream>
int main()
{
    std::string baseurl="http://127.0.0.1:1323";
    std::string username;
    std::cout<<"Please enter your username:";
    std::cin>>username;
    NMSClient nmsClient(baseurl,username);
    nmsClient.run(30);
    spdlog::info("NMSClient has finished running.");
    return 0;
}
