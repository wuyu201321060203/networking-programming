#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <vector>
#include <cstdlib>

#include <boost/noncopyable.hpp>

#include <muduo/net/InetAddress.h>
#include <muduo/base/Logging.h>

#define MAX_NUM_WIDTH 5

extern "C"
{
    #include <lua5.1/lua.h>
    #include <lua5.1/lauxlib.h>
    #include <lua5.1/lualib.h>
}

using muduo::net::InetAddress;

class ConfiguringParser:boost::noncopyable
{
public:

    ConfiguringParser(){}

    void doParse(std::string filePath)
    {
        lua_State *L = lua_open();
        luaL_openlibs(L);
        if(luaL_loadfile(L, filePath.c_str()) || lua_pcall(L, 0 , 0 , 0))
        {
            LOG_FATAL << "cannot run configuration file: " <<
                          lua_tostring(L , -1);
        }
        lua_getglobal(L , "InetAddress");
        if(!lua_istable(L , -1))
        {
            LOG_FATAL << "InetAddress is not a valid table";
        }
        for(int i = 1 ; ; ++i)
        {
            lua_pushnumber(L , i);
            lua_gettable(L , -2);
            if(lua_istable(L , -1))
            {
                std::string ip = getField("ip" , L);
                std::string portStr = getField("port", L);
                uint16_t port = static_cast<uint16_t>(atoi(portStr.c_str()));
                backendServer_.push_back(InetAddress(ip , port));
                lua_pop(L , 1);
            }
            else
                break;
        }
    }

    InetAddress const& getBackendServer(size_t pos) const
    {
        return backendServer_[pos];
    }

    int getHostListSize() const
    {
        return backendServer_.size();
    }

private:

    std::vector<InetAddress> backendServer_;

private:

    std::string getField(char const* key , lua_State* L)
    {
        std::string result;
        lua_pushstring(L , key);
        lua_gettable(L , -2);
        if(!lua_isstring(L , -1))
        {
            LOG_FATAL << "invalid component in background color";
        }
        result = static_cast<char const*>(lua_tostring(L, -1));
        lua_pop(L , 1);
        return result;
    }
};

#endif