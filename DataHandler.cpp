#include "DataHandler.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
DataHandler::DataHandler(const std::string &baseurl):baseurl(baseurl){}
bool DataHandler::sendRequest(const std::string &endpoint,const std::string &data,std::string &response,const std::string &token)
{
    CURL *curl=curl_easy_init();
    if(!curl)
    {
        spdlog::error("Failed to initialize CURL.");
        return false;
    }
    std::string url=baseurl+endpoint;
    curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
    if(!data.empty()) curl_easy_setopt(curl,CURLOPT_POSTFIELDS,data.c_str());
    struct curl_slist *headers=NULL;
    if(!token.empty())
    {
        headers=curl_slist_append(headers,("Authorization: Bearer "+token).c_str());
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
    }
    curl_easy_setopt(curl,CURLOPT_BUFFERSIZE,102400L);
    curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1L);
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"curl/8.2.1");
    curl_easy_setopt(curl,CURLOPT_MAXREDIRS,50L);
    curl_easy_setopt(curl,CURLOPT_HTTP_VERSION,(long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl,CURLOPT_FTP_SKIP_PASV_IP,1L);
    curl_easy_setopt(curl,CURLOPT_TCP_KEEPALIVE,1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,+[](void *ptr,size_t size,size_t nmemb,std::string *data){data->append((char*)ptr,size*nmemb);return size*nmemb;});
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res=curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (headers) curl_slist_free_all(headers);
    if (res!=CURLE_OK)
    {
        spdlog::error("CURL request failed:{}",curl_easy_strerror(res));
        return false;
    }
    return true;
}
std::string DataHandler::getCode(const std::string &token)
{
    std::string response;
    if(sendRequest("/api/info","",response,token))
    {
        try
        {
            auto json=nlohmann::json::parse(response);
            if(json.contains("code")&&json["code"].is_string())
            {
                std::string code=json["code"];
                spdlog::info("Fetched code:{}",code);
                return code;
            }
            else spdlog::error("Invaild 'code' field in info response:expected string,got {}",json.dump());
        }
        catch(const nlohmann::json::exception &e) {spdlog::error("Failed to parse info response:{}",e.what());}
    }
    return "";
}
bool DataHandler::submitCode(const std::string &token,const std::string &code)
{
    std::string response;
    std::string data="code="+code;
    if(sendRequest("/api/validate",data,response,token))
    {
        spdlog::info("Code submitted successfully:{}",code);
        return true;
    }
    spdlog::error("Failed to submit code:{}",code);
    return false;
}




