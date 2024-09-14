#include "AuthManager.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
AuthManager::AuthManager(const std::string &baseurl):baseurl(baseurl){}
bool AuthManager::sendRequest(const std::string &endpoint,const std::string &data,std::string &response,bool useAuth)
{
    CURL *curl=curl_easy_init();
    if(!curl)
    {
        spdlog::error("Failed to initialize CURL.");
        return false;
    }
    std::string url=baseurl+endpoint;
    curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
    struct curl_slist *headers=NULL;
    if(useAuth&&!token.empty()) headers=curl_slist_append(headers,("Authorization: Bearer "+token).c_str());
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
    if(!data.empty()) curl_easy_setopt(curl,CURLOPT_POSTFIELDS,data.c_str());
    curl_easy_setopt(curl,CURLOPT_BUFFERSIZE,102400L);
    curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1L);
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"curl/8.2.1");
    curl_easy_setopt(curl,CURLOPT_MAXREDIRS,50l);
    curl_easy_setopt(curl,CURLOPT_HTTP_VERSION,(long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl,CURLOPT_FTP_SKIP_PASV_IP,1L);
    curl_easy_setopt(curl,CURLOPT_TCP_KEEPALIVE,1L);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,+[](void *ptr,size_t size,size_t nmemb,std::string *data){data->append((char*)ptr,size*nmemb);return size*nmemb;});
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
    CURLcode res=curl_easy_perform(curl);
    if (headers) curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res!=CURLE_OK)
    {
        spdlog::error("CURL request failed:{}",curl_easy_strerror(res));
        return false;
    }
    return true;
}
bool AuthManager::signup(const std::string &username)
{
    this->username=username;
    std::string response;
    if(sendRequest("/signup","username="+username,response,false))
    {
        try
        {
            auto json=nlohmann::json::parse(response);
            if(json.contains("password")&&json["password"].is_string())
            {
                password=json["password"];
                spdlog::info("Signup successful.Password:{}",response);
                return true;
            }
            else spdlog::error("Invalid signup response:{}",response);
        }
        catch(const std::exception &e) {spdlog::error("Failed to parse signup response:{}",e.what());}
    }
    return false;
}
bool AuthManager::login(const std::string &username,const std::string &password)
{
    std::string response;
    if(sendRequest("/login","username"+username+"&password="+password,response,false))
    {
        try
        {
            auto json=nlohmann::json::parse(response);
            if(json.contains("token")&&json["token"].is_string())
            {
                token=json["token"];
                spdlog::info("Login successful.Token:{}",token);
                return true;
            }
            else spdlog::error("Invalid login response:{}",response);
        }
        catch(const std::exception &e) {spdlog::error("Failed to parse login response:{}",e.what());}
    }
    return false;
}
bool AuthManager::refreshToken()
{
    std::string response;
    if(sendRequest("/api/heartbeat","",response,true))
    {
        try
        {
            auto json=nlohmann::json::parse(response);
            if(json.contains("token")&&json["token"].is_string())
            {
                token=json["token"];
                spdlog::info("Token refreshed successfully.");
                return true;
            }
            else spdlog::error("Invalid heartbeat response:{}",response);
        }
        catch(const std::exception &e) {spdlog::error("Failed to parse heartbeat response:{}",e.what());}
    }
    return false;
}
std::string AuthManager::getToken() const{return token;}
std::string AuthManager::getPassword() const{return password;}





