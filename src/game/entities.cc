#include "game.hh"

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

#ifndef STANDALONE
    vector<extentity *> ents;

    vector<extentity *> &getents() { return ents; }

    bool mayattach(extentity &e) { return false; }
    bool attachent(extentity &e, extentity &a) { return false; }

    const char *itemname(int i)
    {
        return NULL;
    }

    int itemicon(int i)
    {
        return -1;
    }

    const char *entmdlname(int type)
    {
        return NULL;
    }

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

    void trypickup(int n, gameent *d)
    {
    }

    void checkitems(gameent *d)
    {
    }

    void putitems(packetbuf &p)            // puts items in network stream and also spawns them locally
    {
    }

    void resetspawns() { }

    void spawnitems(bool force)
    {
    }

    void setspawn(int i, bool on) { if(ents.inrange(i)) { extentity *e = ents[i]; e->setspawned(on); e->clearnopickup(); } }

    extentity *newentity() { return new gameentity(); }
    void deleteentity(extentity *e) { delete (gameentity *)e; }

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
        static const char * const entnames[MAXENTTYPES] =
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
#endif
}

