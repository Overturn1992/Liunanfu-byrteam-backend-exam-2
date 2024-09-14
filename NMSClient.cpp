#include "NMSClient.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
NMSClient::NMSClient(const std::string &baseurl,const std::string &username):authManager(baseurl),dataHandler(baseurl),username(username),keepRunning(true){}
NMSClient::~NMSClient()
{
    keepRunning=false;
    if(tokenRefreshThread.joinable()) tokenRefreshThread.join();
}
void NMSClient::run(int maxMinutes)
{
    if(!authManager.signup(username))
    {
        spdlog::warn("Signed failed,we will try to re-signup in 5 seconds.");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        retrySignup();
    }
    if(!authManager.login(username,authManager.getPassword()))
    {
        spdlog::warn("Login failed,we will try to re-login in 5 seconds.");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        retryLogin();
    }
    tokenRefreshThread=std::thread(&NMSClient::refreshTokenLoop,this);
    int submissionCount=0;
    auto startTime=std::chrono::steady_clock::now();
    while(std::chrono::steady_clock::now()-startTime<std::chrono::minutes(maxMinutes))
    {
        if(submitData()) submissionCount++;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    keepRunning=false;
    if(tokenRefreshThread.joinable()) tokenRefreshThread.join();
    spdlog::info("NMSClient completed with {} submissions.",submissionCount);
}
void NMSClient::refreshTokenLoop()
{
    while(keepRunning)
    {
        if(!authManager.refreshToken())
        {
            spdlog::warn("Failed to refresh token,retrying login.");
            while(!authManager.login(username,authManager.getPassword()))
            {
                spdlog::error("Re-login failed,retrying in 10 seconds...");
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
bool NMSClient::submitData()
{
    std::string token=authManager.getToken();
    std::string code=dataHandler.getCode(token);
    if(code.empty())
    {
        spdlog::warn("Failed to fetch code,retrying in 10 seconds.");
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return false;
    }
    if(!dataHandler.submitCode(token,code))
    {
        spdlog::warn("Failed to submit code,retrying in 10 seconds.");
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return false;
    }
    return true;
}
void NMSClient::retrySignup()
{
    if(!authManager.signup(username))
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        retrySignup();
    }
}
void NMSClient::retryLogin()
{
    if(!authManager.login(username,authManager.getPassword()))
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        retryLogin();
    }
}





