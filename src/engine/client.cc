// client.cpp, mostly network related client game code

#include "engine.hh"

bool multiplayer(bool msg)
{
    return false;
}

bool isconnected(bool attempt, bool local)
{
    return haslocalclients();
}

void trydisconnect()
{
    if(haslocalclients()) localdisconnect();
    else conoutf("not connected");
}

ICOMMAND(disconnect, "", (), trydisconnect());
ICOMMAND(localconnect, "", (), { if(!isconnected()) localconnect(); });
ICOMMAND(localdisconnect, "", (), { if(haslocalclients()) localdisconnect(); });

void clientkeepalive() {}

void gets2c() {}

void neterr(const char *s, bool disc)
{
    conoutf(CON_ERROR, "\f3illegal network message (%s)", s);
    if(disc) localdisconnect();
}
