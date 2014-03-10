#include <native.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace native::http;

int main(int argc, const char** argv, const char** envptr)
{
    std::vector<std::string> argsList;
    std::map<std::string, std::string> envMap;
    //TODO: add debug future
    bool debug(false);
    for(int i = 0; i < argc; ++i)
    {
        std::string argStr(argv[i]);
        debug && std::cout<<"adding arg: "<<argStr<<"\n";
        argsList.push_back(argStr);
    };
    
    for(const char** envPtrTmp = envptr; *envPtrTmp != 0; ++envPtrTmp)
    {
        const char* envItemPtr = *envPtrTmp;
        const char* b = envItemPtr;
        while(*envItemPtr && *envItemPtr != '=')
        {
            ++envItemPtr;
        }
        std::string envName(b, envItemPtr);
        if(*envItemPtr == '=')
        {
            ++envItemPtr;
        }
        std::string varVal(envItemPtr);
        debug && std::cout<<"adding env: "<<envName<<"=\""<<varVal<<"\"\n";
        envMap.insert(std::make_pair(envName, varVal));
    }
    http server;
    int port = 8080;

    std::vector<std::string>::iterator portVarItem(std::find(argsList.begin(), argsList.end(), "-p"));
    if((portVarItem != argsList.end())&& (++portVarItem != argsList.end()))
    {
        int envPort = atoi((*portVarItem).c_str());
        if((envPort>0)&&(envPort != port))
        {
            port = envPort;
        }
    }

    if(!server.listen("0.0.0.0", port, [](request& req, response& res)
        {
            std::string body = req.get_body(); // Now you can write a custom handler for the body content.
            res.set_status(200);
            res.set_header("Content-Type", "text/plain");
            res.end("C++ FTW\n");
        }
    )) return 1; // Failed to run server.

    std::cout << "Server running at http://0.0.0.0:" << port << "/" << std::endl;
    return native::run();
}
