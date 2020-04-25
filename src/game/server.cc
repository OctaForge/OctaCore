#include "game.hh"

namespace game
{
    const char *gameident() { return "Tesseract"; }
}

namespace server
{
 
    string smapname = "";

    void serverinit()
    {
        smapname[0] = '\0';
    }

    int numclients(int exclude = -1, bool nospec = true, bool noai = true, bool priv = false)
    {
        return 0;
    }

    int spawntime(int type)
    {
        return 0;
    }

    bool delayspawn(int type)
    {
        return false;
    }

    bool ispaused() { return false; }

    int scaletime(int t) { return t*100; }

    void serverupdate()
    {
    }
 
    void noclients()
    {
    }

    void localconnect(int n)
    {
    }

    void localdisconnect(int n)
    {
    }

    int clientconnect(int n, uint ip)
    {
        return DISC_LOCAL;
    }

    void clientdisconnect(int n)
    {
    }

    int reserveclients() { return 3; }

    bool allowbroadcast(int n)
    {
        return true;
    }

    int laninfoport() { return TESSERACT_LANINFO_PORT; }
    int serverport() { return TESSERACT_SERVER_PORT; }
    int numchannels() { return 3; }

    int protocolversion() { return 0; }
}

