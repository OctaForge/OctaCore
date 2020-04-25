#include "game.hh"

namespace game
{
    static int maptime = 0, maprealtime = 0;

    gameent *player1 = NULL;         // our client

    const char *getclientmap() { return clientmap; }

    void resetgamestate()
    {
    }

    gameent *spawnstate(gameent *d)              // reset player state not persistent accross spawns
    {
        d->respawn();
        d->spawnstate(0);
        return d;
    }

    void respawnself()
    {
        if(ispaused()) return;
        spawnplayer(player1);
    }

    gameent *pointatplayer()
    {
        return NULL;
    }

    gameent *hudplayer()
    {
        return player1;
    }

    void setupcamera()
    {
    }

    bool allowthirdperson()
    {
        return true;
    }

    bool detachcamera()
    {
        return player1->state == CS_DEAD;
    }

    bool collidecamera()
    {
        return (player1->state != CS_EDITING);
    }

    void updateworld()        // main game update loop
    {
        if(!maptime) { maptime = lastmillis; maprealtime = totalmillis; return; }
        if(!curtime) { gets2c(); if(player1->clientnum>=0) c2sinfo(); return; }

        physicsframe();
        moveragdolls();
        gets2c();
        if(connected)
        {
            crouchplayer(player1, 10, true);
            moveplayer(player1, 10, true);
            entities::checkitems(player1);
        }
        if(player1->clientnum>=0) c2sinfo();   // do this last, to reduce the effective frame lag
    }

    void spawnplayer(gameent *d)   // place at random spawn
    {
        findplayerspawn(d, -1, 0);
        spawnstate(d);
        if(d==player1)
        {
            if(editmode) d->state = CS_EDITING;
            else if(d->state != CS_SPECTATOR) d->state = CS_ALIVE;
        }
        else d->state = CS_ALIVE;
    }

    VARP(spawnwait, 0, 0, 1000);

    void respawn()
    {
        if(player1->state==CS_DEAD)
        {
            respawnself();
        }
    }
    COMMAND(respawn, "");

    // inputs

    VARP(jumpspawn, 0, 1, 1);

    bool canjump()
    {
        if(!connected) return false;
        if(jumpspawn) respawn();
        return player1->state!=CS_DEAD;
    }

    bool cancrouch()
    {
        if(!connected) return false;
        return player1->state!=CS_DEAD;
    }

    bool allowmove(physent *d)
    {
        return true;
    }

    gameent *getclient(int cn)   // ensure valid entity
    {
        if(cn == player1->clientnum) return player1;
        return NULL;
    }

    void initclient()
    {
        player1 = spawnstate(new gameent);
        filtertext(player1->name, "unnamed", false, false, MAXNAMELEN);
    }

    VARP(showmodeinfo, 0, 1, 1);

    void startgame()
    {
        // reset perma-state
        player1->startgame();

        maptime = maprealtime = 0;

        syncplayer();

        disablezoom();

        execident("mapstart");
    }

    void startmap(const char *name)   // called just after a map load
    {
        spawnplayer(player1);
        entities::resetspawns();
        copystring(clientmap, name ? name : "");

        sendmapinfo();
    }

    const char *getmapinfo()
    {
        return NULL;
    }

    const char *getscreenshotinfo()
    {
        return NULL;
    }

    void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
    {
    }

    void dynentcollide(physent *d, physent *o, const vec &dir)
    {
    }

    void msgsound(int n, physent *d)
    {
        playsound(n);
    }

    int numdynents() { return 1; }

    dynent *iterdynents(int i)
    {
        if(!i) return player1;
        return NULL;
    }

    bool needminimap() { return false; }

    void drawicon(int icon, float x, float y, float sz)
    {
    }

    float abovegameplayhud(int w, int h)
    {
        return 1;
    }

    void drawhudicons(gameent *d)
    {
    }

    void gameplayhud(int w, int h)
    {
        pushhudscale(h/1800.0f);

        gameent *d = hudplayer();
        if(d->state!=CS_EDITING)
        {
            if(d->state!=CS_SPECTATOR) drawhudicons(d);
        }

        pophudmatrix();
    }

    float clipconsole(float w, float h)
    {
        return 0;
    }

    const char *defaultcrosshair(int index)
    {
        return "media/interface/crosshair/default.png";
    }

    int selectcrosshair(vec &)
    {
        return 0;
    }

    // any data written into this vector will get saved with the map data. Must take care to do own versioning, and endianess if applicable. Will not get called when loading maps from other games, so provide defaults.
    void writegamedata(vector<char> &extras) {}
    void readgamedata(vector<char> &extras) {}

    const char *gameconfig() { return "config/game.cfg"; }
    const char *savedconfig() { return "config/saved.cfg"; }
    const char *restoreconfig() { return "config/restore.cfg"; }
    const char *defaultconfig() { return "config/default.cfg"; }
    const char *autoexec() { return "config/autoexec.cfg"; }
    const char *savedservers() { return "config/servers.cfg"; }

    void loadconfigs()
    {
        execfile("config/auth.cfg", false);
    }
}

