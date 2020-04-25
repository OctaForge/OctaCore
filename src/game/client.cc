#include "game.hh"

extern void clearmainmenu();

namespace game
{
    VARP(minradarscale, 0, 384, 10000);
    VARP(maxradarscale, 1, 1024, 10000);
    VARP(radarteammates, 0, 1, 1);
    FVARP(minimapalpha, 0, 1, 1);

    float calcradarscale()
    {
        return clamp(max(minimapradius.x, minimapradius.y)/3, float(minradarscale), float(maxradarscale));
    }

    void drawminimap(gameent *d, float x, float y, float s)
    {
        vec pos = vec(d->o).sub(minimapcenter).mul(minimapscale).add(0.5f), dir;
        vecfromyawpitch(camera1->yaw, 0, 1, 0, dir);
        float scale = calcradarscale();
        gle::defvertex(2);
        gle::deftexcoord0();
        gle::begin(GL_TRIANGLE_FAN);
        loopi(16)
        {
            vec v = vec(0, -1, 0).rotate_around_z(i/16.0f*2*M_PI);
            gle::attribf(x + 0.5f*s*(1.0f + v.x), y + 0.5f*s*(1.0f + v.y));
            vec tc = vec(dir).rotate_around_z(i/16.0f*2*M_PI);
            gle::attribf(1.0f - (pos.x + tc.x*scale*minimapscale.x), pos.y + tc.y*scale*minimapscale.y);
        }
        gle::end();
    }

    void setradartex()
    {
        settexture("media/interface/radar/radar.png", 3);
    }

    bool connected = false, remote = false;
    int sessionid = 0;
    string servdesc = "", servauth = "";

    VARP(deadpush, 1, 2, 20);

    void sendmapinfo()
    {
    }

    void writeclientinfo(stream *f)
    {
    }

    bool allowedittoggle()
    {
        return true;
    }

    void edittoggled(bool on)
    {
        disablezoom();
        player1->respawned = -2;
    }

    string clientmap = "";

    void changemapserv(const char *name, int mode)        // forced map change from the server
    {
        if(editmode) toggleedit();
        if(!name[0] || !load_world(name))
        {
            emptymap(0, true, name);
        }
        startgame();
    }


    void changemap(const char *name, int mode) // request map change, server may ignore
    {
        changemapserv(name, 0);
        localconnect();
        clearmainmenu(); /* XXX hack */
        connected = true;
    }
    void changemap(const char *name)
    {
        changemap(name, 0);
    }
    ICOMMAND(map, "s", (char *name), changemap(name));

    void forceedit(const char *name)
    {
        changemap(name, 0);
    }

    void newmap(int size)
    {
        if(size>=0) emptymap(size, true, NULL);
        else enlargemap(true);
        localconnect();
        connected = true;
    }

    void edittrigger(const selinfo &sel, int op, int arg1, int arg2, int arg3, const VSlot *vs)
    {
    }

    void vartrigger(ident *id)
    {
    }

    bool ispaused() { return false; }
    bool allowmouselook() { return true; }

    int scaletime(int t) { return t*100; }

    void connectfail()
    {
    }

    void gameconnect(bool _remote)
    {
        remote = _remote;
    }

    void gamedisconnect(bool cleanup)
    {
        connected = remote = false;
        player1->clientnum = -1;
        if(editmode) toggleedit();
        sessionid = 0;
        player1->respawn();
        player1->state = CS_ALIVE;
        if(cleanup)
        {
            clientmap[0] = '\0';
        }
    }

    void toserver(char *text) { conoutf(CON_CHAT, "%s", text); }
}

