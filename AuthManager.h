#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H
#include <string>
class AuthManager
{
public:
    explicit AuthManager(const std::string &baseurl);
    bool signup(const std::string &username);
    bool login(const std::string &username,const std::string &password);
    bool refreshToken();
    std::string getToken() const;
    std::string getPassword() const;
private:
    std::string baseurl;
    std::string token;
    std::string password;
    std::string username;
    bool sendRequest(const std::string &endpoint,const std::string &data,std::string &response,bool useAuth);
};
#endif //AUTHMANAGER_H
