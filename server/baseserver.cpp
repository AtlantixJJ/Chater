#include "baseserver.h"
#include "serverconfig.h"

BaseServer::BaseServer(string config_fname)
{
    sc = new ServerConfig(config_fname);
}

BaseServer::BaseServer(const ServerConfig &sc)
{
    this->sc = new ServerConfig(sc);
}

BaseServer::~BaseServer()
{
    delete [] sc;
}