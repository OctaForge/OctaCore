#include "cube.hh"

#include <engine/worldio.hh>
#include <engine/world.hh>

extern void clearmainmenu();

namespace game
{
    dynent *player1 = NULL;         // our client
    string clientmap = "";
    bool connected = false;

    const char *getclientmap() { return clientmap; }

    const char *gameident() { return "OctaForge"; }

    void rendergame()
    {
    }

    void renderavatar()
    {
    }

    void preloadweapons()
    {
    }

    void preloadsounds()
    {
    }

    void preload()
    {
    }

    void resetgamestate()
    {
    }

    dynent *spawnstate(dynent *d)              // reset player state not persistent accross spawns
    {
        d->reset();
        return d;
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
        if(!curtime) { return; }

        physicsframe();
        if(connected)
        {
            crouchplayer(player1, 10, true);
            moveplayer(player1, 10, true);
        }
    }

    void spawnplayer(dynent *d)   // place at random spawn
    {
        d->o.x = d->o.y = d->o.z = 0.5f*getworldsize();
        d->o.z += 1;
        entinmap(d);
        spawnstate(d);
        if(d==player1)
        {
            if(editmode) d->state = CS_EDITING;
            else if(d->state != CS_SPECTATOR) d->state = CS_ALIVE;
        }
        else d->state = CS_ALIVE;
    }

    // inputs

    bool canjump()
    {
        return connected;
    }

    bool cancrouch()
    {
        return connected;
    }

    bool allowmove(physent *d)
    {
        return connected;
    }

    void initclient()
    {
        player1 = spawnstate(new dynent);
        player1->reset();
    }

    VARP(showmodeinfo, 0, 1, 1);

    void startgame()
    {
        disablezoom();

        execident("mapstart");
    }

    void startmap(const char *name)   // called just after a map load
    {
        spawnplayer(player1);
        copystring(clientmap, name ? name : "");
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

    void drawhudicons(dynent *d)
    {
    }

    void gameplayhud(int w, int h)
    {
        pushhudscale(h/1800.0f);

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

    void drawminimap(dynent *d, float x, float y, float s)
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
    }

    void changemap(const char *name)
    {
        if(editmode) toggleedit();
        if(!name[0] || !load_world(name))
        {
            emptymap(0, true, name);
        }
        startgame();
        connected = true;
        clearmainmenu(); /* XXX hack */
    }
    ICOMMAND(map, "s", (char *name), changemap(name));

    void forceedit(const char *name)
    {
        changemap(name);
    }

    void newmap(int size)
    {
        if(size>=0) emptymap(size, true, NULL);
        else enlargemap(true);
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

    void gamedisconnect(bool cleanup)
    {
        if (!connected) {
            return;
        }
        connected = false;
        if(editmode) toggleedit();
        player1->reset();
        player1->state = CS_ALIVE;
        if(cleanup)
        {
            clientmap[0] = '\0';
        }
    }

    void toserver(char *text) { }
}

namespace entities
{
    using namespace game;

    int extraentinfosize() { return 0; }       // size in bytes of what the 2 methods below read/write... so it can be skipped by other games

    void writeent(entity &e, char *buf)   // write any additional data to disk (except for ET_ ents)
    {
    }

    void readent(entity &e, char *buf, int ver)     // read from disk, and init
    {
    }

    vector<extentity *> ents;

    vector<extentity *> &getents() { return ents; }

    bool mayattach(extentity &e) { return false; }
    bool attachent(extentity &e, extentity &a) { return false; }

    const char *entmodel(const entity &e)
    {
        return NULL;
    }

    void preloadentities()
    {
    }

    void renderentities()
    {
    }

    extentity *newentity() { return new extentity(); }
    void deleteentity(extentity *e) { delete e; }

    void clearents()
    {
        while(ents.length()) deleteentity(ents.pop());
    }

    void animatemapmodel(const extentity &e, int &anim, int &basetime)
    {
    }

    void fixentity(extentity &e)
    {
    }

    void entradius(extentity &e, bool color)
    {
    }

    bool printent(extentity &e, char *buf, int len)
    {
        return false;
    }

    const char *entnameinfo(entity &e) { return ""; }
    const char *entname(int i)
    {
        static const char * const entnames[ET_GAMESPECIFIC] =
        {
            "none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight", "decal",
        };
        return i>=0 && size_t(i)<sizeof(entnames)/sizeof(entnames[0]) ? entnames[i] : "";
    }

    void editent(int i, bool local)
    {
    }

    float dropheight(entity &e)
    {
        return 4.0f;
    }
}

bool haslocalclients() { return game::connected; }
