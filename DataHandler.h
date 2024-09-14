#ifndef DATAHANDLER_H
#define DATAHANDLER_H
#include <string>
class DataHandler
{
public:
    explicit DataHandler(const std::string &baseurl);
    std::string getCode(const std::string &token);
    bool submitCode(const std::string &token,const std::string &code);
private:
    std::string baseurl;
    bool sendRequest(const std::string &endpoint,const std::string &data,std::string &response,const std::string &token="");
};
#endif //DATAHANDLER_H
