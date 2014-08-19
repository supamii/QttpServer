#include <native/native.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace native::http;

namespace
{
    std::vector<std::string>& geArgList()
    {
        static std::vector<std::string> args;
        return args;
    }
    std::map<std::string, std::string>& geEnvMap()
    {
        static std::map<std::string, std::string> env;
        return env;
    }
    void parseArgsAndEnvs(int argc, const char** argv, const char** envptr)
    {
        //TODO: add debug future
        bool debug(false);
        for(int i = 0; i < argc; ++i)
        {
            std::string argStr(argv[i]);
            debug && std::cout<<"adding arg: "<<argStr<<"\n";
            geArgList().push_back(argStr);
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
            geEnvMap().insert(std::make_pair(envName, varVal));
        }
    }

    int run_server()
    {
        std::shared_ptr<http> server(new http);
        int port = 8080;

        std::vector<std::string>::iterator portVarItem(std::find(geArgList().begin(), geArgList().end(), "-p"));
        if((portVarItem != geArgList().end())&& (++portVarItem != geArgList().end()))
        {
            int envPort = atoi((*portVarItem).c_str());
            if((envPort>0)&&(envPort != port))
            {
                port = envPort;
            }
        }

        if(!server->listen("0.0.0.0", port, [server](request& req, response& res)
            {
                static int n = 0;
                ++n;
                std::string body = req.get_body(); // Now you can write a custom handler for the body content.
                res.set_status(200);
                res.set_header("Content-Type", "text/plain");
                std::ostringstream res_str;
                res_str<<"C++ FTW "<<n<<"\n";
                res.end(res_str.str());
            }
        )) return 1; // Failed to run server.

        std::cout << "Server running at http://0.0.0.0:" << port << "/" << std::endl;
        return 0;
    }
}

int main(int argc, const char** argv, const char** envptr)
{
    parseArgsAndEnvs(argc, argv, envptr);
    int err = run_server();
    if(err != 0)
    {
        return err;
    }
    return native::run();
}
