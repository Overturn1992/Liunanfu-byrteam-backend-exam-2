cmake_minimum_required(VERSION 3.29)
project(byrteam)

set(CMAKE_CXX_STANDARD 20)

add_executable(byrteam main.cpp AuthManager.cpp AuthManager.h DataHandler.cpp DataHandler.h
        NMSClient.cpp
        NMSClient.h)

find_package(CURL REQUIRED)
target_link_libraries(byrteam PRIVATE CURL::libcurl)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(byrteam PRIVATE nlohmann_json::nlohmann_json)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(byrteam PRIVATE Threads::Threads)

find_package(spdlog REQUIRED)
target_link_libraries(byrteam PRIVATE spdlog::spdlog)
