#include "game.hh"

namespace game
{
    void parseoptions(vector<const char *> &args)
    {
        loopv(args)
#ifndef STANDALONE
            if(!game::clientoption(args[i]))
#endif
            if(!server::serveroption(args[i]))
                conoutf(CON_ERROR, "unknown command-line option: %s", args[i]);
    }

    const char *gameident() { return "Tesseract"; }
}

namespace server
{
    struct clientinfo;

    struct servstate : gamestate
    {
        vec o;
        int state, editstate;
        int lastspawn, lifesequence;
        int lasttimeplayed, timeplayed;
        float effectiveness;

        servstate() : state(CS_DEAD), editstate(CS_DEAD), lifesequence(0) {}

        void reset()
        {
            if(state!=CS_SPECTATOR) state = editstate = CS_DEAD;

            timeplayed = 0;
            effectiveness = 0;

            respawn();
        }

        void respawn()
        {
            gamestate::respawn();
            o = vec(-1e10f, -1e10f, -1e10f);
            lastspawn = -1;
        }

        void reassign()
        {
            respawn();
        }
    };

    extern int gamemillis;

    struct clientinfo
    {
        int clientnum, ownernum, connectmillis, sessionid, overflow;
        string name;
        bool connected, local, timesync;
        servstate state;
        vector<uchar> messages;
        uchar *wsdata;
        int wslen;
        string clientmap;
        bool warned;

        clientinfo() { reset(); }

        void mapchange()
        {
            state.reset();
            overflow = 0;
            timesync = false;
            clientmap[0] = '\0';
            warned = false;
        }

        void reassign()
        {
            state.reassign();
            timesync = false;
        }

        void reset()
        {
            name[0] = 0;
            connected = local = false;
            messages.setsize(0);
            mapchange();
        }
    };

    int gamemillis = 0;
    bool shouldstep = true;

    string smapname = "";
    enet_uint32 lastsend = 0;

    vector<clientinfo *> connects, clients;

    void *newclientinfo() { return new clientinfo; }
    void deleteclientinfo(void *ci) { delete (clientinfo *)ci; }

    clientinfo *getinfo(int n)
    {
        if(n < MAXCLIENTS) return (clientinfo *)getclientinfo(n);
        return NULL;
    }

    int msgsizelookup(int msg)
    {
        static int sizetable[NUMMSG] = { -1 };
        if(sizetable[0] < 0)
        {
            memset(sizetable, -1, sizeof(sizetable));
            for(const int *p = msgsizes; *p >= 0; p += 2) sizetable[p[0]] = p[1];
        }
        return msg >= 0 && msg < NUMMSG ? sizetable[msg] : -1;
    }

    void sendservmsg(const char *) {}

    bool serveroption(const char *arg)
    {
        return false;
    }

    void serverinit()
    {
        smapname[0] = '\0';
    }

    int numclients(int exclude = -1, bool nospec = true, bool noai = true, bool priv = false)
    {
        int n = 0;
        loopv(clients)
        {
            clientinfo *ci = clients[i];
            if(ci->clientnum!=exclude && (!nospec || ci->state.state!=CS_SPECTATOR || (priv && ci->local))) n++;
        }
        return n;
    }

    bool duplicatename(clientinfo *ci, const char *name)
    {
        if(!name) name = ci->name;
        loopv(clients) if(clients[i]!=ci && !strcmp(name, clients[i]->name)) return true;
        return false;
    }

    int spawntime(int type)
    {
        int np = numclients(-1, true, false);
        np = np<3 ? 4 : (np>4 ? 2 : 3);         // spawn times are dependent on number of players
        int sec = 0;
        switch(type)
        {
        }
        return sec*1000;
    }

    bool delayspawn(int type)
    {
        switch(type)
        {
            default:
                return false;
        }
    }

    int welcomepacket(packetbuf &p, clientinfo *ci);
    void sendwelcome(clientinfo *ci);

    bool ispaused() { return false; }

    int scaletime(int t) { return t*100; }

    int checktype(int type, clientinfo *ci)
    {
        return type;
    }

    struct worldstate
    {
        int uses, len;
        uchar *data;

        worldstate() : uses(0), len(0), data(NULL) {}

        void setup(int n) { len = n; data = new uchar[n]; }
        void cleanup() { DELETEA(data); len = 0; }
        bool contains(const uchar *p) const { return p >= data && p < &data[len]; }
    };
    vector<worldstate> worldstates;
    bool reliablemessages = false;

    void cleanworldstate(ENetPacket *packet)
    {
        loopv(worldstates)
        {
            worldstate &ws = worldstates[i];
            if(!ws.contains(packet->data)) continue;
            ws.uses--;
            if(ws.uses <= 0)
            {
                ws.cleanup();
                worldstates.removeunordered(i);
            }
            break;
        }
    }

    static void sendmessages(worldstate &ws, ucharbuf &wsbuf)
    {
        if(wsbuf.empty()) return;
        int wslen = wsbuf.length();
        wsbuf.put(wsbuf.buf, wslen);
        loopv(clients)
        {
            clientinfo &ci = *clients[i];
            uchar *data = wsbuf.buf;
            int size = wslen;
            if(ci.wsdata >= wsbuf.buf) { data = ci.wsdata + ci.wslen; size -= ci.wslen; }
            if(size <= 0) continue;
            ENetPacket *packet = enet_packet_create(data, size, (reliablemessages ? ENET_PACKET_FLAG_RELIABLE : 0) | ENET_PACKET_FLAG_NO_ALLOCATE);
            sendpacket(ci.clientnum, 1, packet);
            if(packet->referenceCount) { ws.uses++; packet->freeCallback = cleanworldstate; }
            else enet_packet_destroy(packet);
        }
        wsbuf.offset(wsbuf.length());
    }

    static inline void addmessages(worldstate &ws, ucharbuf &wsbuf, int mtu, clientinfo &bi, clientinfo &ci)
    {
        if(bi.messages.empty()) return;
        if(wsbuf.length() + 10 + bi.messages.length() > mtu) sendmessages(ws, wsbuf);
        int offset = wsbuf.length();
        putint(wsbuf, N_CLIENT);
        putint(wsbuf, bi.clientnum);
        putuint(wsbuf, bi.messages.length());
        wsbuf.put(bi.messages.getbuf(), bi.messages.length());
        bi.messages.setsize(0);
        int len = wsbuf.length() - offset;
        if(ci.wsdata < wsbuf.buf) { ci.wsdata = &wsbuf.buf[offset]; ci.wslen = len; }
        else ci.wslen += len;
    }

    bool buildworldstate()
    {
        int wsmax = 0;
        loopv(clients)
        {
            clientinfo &ci = *clients[i];
            ci.overflow = 0;
            ci.wsdata = NULL;
            if(ci.messages.length()) wsmax += 10 + ci.messages.length();
        }
        if(wsmax <= 0)
        {
            reliablemessages = false;
            return false;
        }
        worldstate &ws = worldstates.add();
        ws.setup(2*wsmax);
        int mtu = getservermtu() - 100;
        if(mtu <= 0) mtu = ws.len;
        ucharbuf wsbuf(ws.data, ws.len);
        loopv(clients)
        {
            clientinfo &ci = *clients[i];
            addmessages(ws, wsbuf, mtu, ci, ci);
        }
        sendmessages(ws, wsbuf);
        reliablemessages = false;
        if(ws.uses) return true;
        ws.cleanup();
        worldstates.drop();
        return false;
    }

    bool sendpackets(bool force)
    {
        if(clients.empty() || !hasnonlocalclients()) return false;
        enet_uint32 curtime = enet_time_get()-lastsend;
        if(curtime<40 && !force) return false;
        bool flush = buildworldstate();
        lastsend += curtime - (curtime%40);
        return flush;
    }

    template<class T>
    void sendstate(servstate &gs, T &p)
    {
        putint(p, gs.lifesequence);
    }

    void spawnstate(clientinfo *ci)
    {
        servstate &gs = ci->state;
        gs.spawnstate(0);
        gs.lifesequence = (gs.lifesequence + 1)&0x7F;
    }

    void sendwelcome(clientinfo *ci)
    {
        packetbuf p(MAXTRANS, ENET_PACKET_FLAG_RELIABLE);
        int chan = welcomepacket(p, ci);
        sendpacket(ci->clientnum, chan, p.finalize());
    }

    bool hasmap(clientinfo *ci)
    {
        return true;
    }

    int welcomepacket(packetbuf &p, clientinfo *ci)
    {
        putint(p, N_WELCOME);
        putint(p, N_MAPCHANGE);
        sendstring(smapname, p);
        return 1;
    }

    void changemap(const char *s, int mode)
    {
        gamemillis = 0;
        copystring(smapname, s);
        loopv(clients)
        {
            clientinfo *ci = clients[i];
            ci->state.timeplayed += lastmillis - ci->state.lasttimeplayed;
        }

        kicknonlocalclients(DISC_LOCAL);

        sendf(-1, 1, "ris", N_MAPCHANGE, smapname);

        loopv(clients)
        {
            clientinfo *ci = clients[i];
            ci->mapchange();
            ci->state.lasttimeplayed = lastmillis;
        }
    }

    void forcemap(const char *map, int mode)
    {
        changemap(map, mode);
    }

    void serverupdate()
    {
        if(shouldstep)
        {
            gamemillis += curtime;
        }

        loopv(connects) if(totalmillis-connects[i]->connectmillis>15000) disconnect_client(connects[i]->clientnum, DISC_TIMEOUT);

        shouldstep = clients.length() > 0;
    }
 
    bool shouldspectate(clientinfo *ci)
    {
        return false;
    }
 
    void unspectate(clientinfo *ci)
    {
    }

    void sendservinfo(clientinfo *ci)
    {
        sendf(ci->clientnum, 1, "ri4", N_SERVINFO, ci->clientnum, PROTOCOL_VERSION, ci->sessionid);
    }

    void noclients()
    {
    }

    void localconnect(int n)
    {
        clientinfo *ci = getinfo(n);
        ci->clientnum = ci->ownernum = n;
        ci->connectmillis = totalmillis;
        ci->sessionid = (rnd(0x1000000)*((totalmillis%10000)+1))&0xFFFFFF;
        ci->local = true;

        connects.add(ci);
        sendservinfo(ci);
    }

    void localdisconnect(int n)
    {
        clientdisconnect(n);
    }

    int clientconnect(int n, uint ip)
    {
        clientinfo *ci = getinfo(n);
        ci->clientnum = ci->ownernum = n;
        ci->connectmillis = totalmillis;
        ci->sessionid = (rnd(0x1000000)*((totalmillis%10000)+1))&0xFFFFFF;

        connects.add(ci);
        return DISC_LOCAL;
    }

    void clientdisconnect(int n)
    {
        clientinfo *ci = getinfo(n);
        if(ci->connected)
        {
            ci->state.timeplayed += lastmillis - ci->state.lasttimeplayed;
            sendf(-1, 1, "ri2", N_CDIS, n);
            clients.removeobj(ci);
            if(!numclients(-1, false, true)) noclients(); // bans clear when server empties
        }
        else connects.removeobj(ci);
    }

    int reserveclients() { return 3; }

    bool allowbroadcast(int n)
    {
        clientinfo *ci = getinfo(n);
        return ci && ci->connected;
    }

    void receivefile(int sender, uchar *data, int len)
    {
    }

    void connected(clientinfo *ci)
    {
        shouldstep = true;

        connects.removeobj(ci);
        clients.add(ci);

        ci->connected = true;
        ci->state.lasttimeplayed = lastmillis;

        sendwelcome(ci);
    }

    void parsepacket(int sender, int chan, packetbuf &p)     // has to parse exactly each byte of the packet
    {
        if(sender<0 || p.packet->flags&ENET_PACKET_FLAG_UNSEQUENCED || chan > 2) return;
        char text[MAXTRANS];
        int type;
        clientinfo *ci = sender>=0 ? getinfo(sender) : NULL, *cq = ci, *cm = ci;
        if(ci && !ci->connected)
        {
            if(chan==0) return;
            else if(chan!=1) { disconnect_client(sender, DISC_MSGERR); return; }
            else while(p.length() < p.maxlen) switch(checktype(getint(p), ci))
            {
                case N_CONNECT:
                {
                    getstring(text, p);
                    filtertext(text, text, false, false, MAXNAMELEN);
                    if(!text[0]) copystring(text, "unnamed");
                    copystring(ci->name, text, MAXNAMELEN+1);
                    connected(ci);
                    break;
                }

                default:
                    disconnect_client(sender, DISC_MSGERR);
                    return;
            }
            return;
        }
        else if(chan==2)
        {
            receivefile(sender, p.buf, p.maxlen);
            return;
        }

        if(p.packet->flags&ENET_PACKET_FLAG_RELIABLE) reliablemessages = true;
        #define QUEUE_AI clientinfo *cm = cq;
        #define QUEUE_MSG { if(cm && (!cm->local || hasnonlocalclients())) while(curmsg<p.length()) cm->messages.add(p.buf[curmsg++]); }
        #define QUEUE_BUF(body) { \
            if(cm && (!cm->local || hasnonlocalclients())) \
            { \
                curmsg = p.length(); \
                { body; } \
            } \
        }
        #define QUEUE_INT(n) QUEUE_BUF(putint(cm->messages, n))
        #define QUEUE_UINT(n) QUEUE_BUF(putuint(cm->messages, n))
        #define QUEUE_STR(text) QUEUE_BUF(sendstring(text, cm->messages))
        int curmsg;
        while((curmsg = p.length()) < p.maxlen) switch(type = checktype(getint(p), ci))
        {
            case N_SPAWN:
            {
                int ls = getint(p);
                if(!cq || (cq->state.state!=CS_ALIVE && cq->state.state!=CS_DEAD && cq->state.state!=CS_EDITING) || ls!=cq->state.lifesequence || cq->state.lastspawn<0) break;
                cq->state.lastspawn = -1;
                cq->state.state = CS_ALIVE;
                QUEUE_AI;
                QUEUE_BUF({
                    putint(cm->messages, N_SPAWN);
                    sendstate(cq->state, cm->messages);
                });
                break;
            }

            case N_NEWMAP:
            {
                int size = getint(p);
                if(!ci->local && ci->state.state==CS_SPECTATOR) break;
                if(size>=0)
                {
                    smapname[0] = '\0';
                }
                QUEUE_MSG;
                break;
            }

            case -1:
                disconnect_client(sender, DISC_MSGERR);
                return;

            case -2:
                disconnect_client(sender, DISC_OVERFLOW);
                return;

            default:
            {
                int size = server::msgsizelookup(type);
                if(size<=0) { disconnect_client(sender, DISC_MSGERR); return; }
                loopi(size-1) getint(p);
                if(cq && (ci != cq || ci->state.state!=CS_SPECTATOR)) { QUEUE_AI; QUEUE_MSG; }
            }
        }
    }

    int laninfoport() { return TESSERACT_LANINFO_PORT; }
    int serverport() { return TESSERACT_SERVER_PORT; }
    int numchannels() { return 3; }

    void serverinforeply(ucharbuf &req, ucharbuf &p)
    {
        putint(p, PROTOCOL_VERSION);
        putint(p, numclients(-1, false, true));
        putint(p, maxclients);
        sendstring(smapname, p);
        sendserverinforeply(p);
    }

    int protocolversion() { return PROTOCOL_VERSION; }
}

