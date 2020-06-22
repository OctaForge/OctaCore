#ifndef ENGINE_MAIN_HH
#define ENGINE_MAIN_HH

#include <shared/tools.hh>

extern int initing, numcpus;
extern int mainmenu;
extern int screenw, screenh;
extern bool minimized;
extern float loadprogress;
extern bool inbetweenframes, renderedframe;

struct dynent;
extern dynent *player;

void clearmainmenu();

enum
{
    NOT_INITING = 0,
    INIT_GAME,
    INIT_LOAD,
    INIT_RESET
};

bool initwarning(const char *desc, int level = INIT_RESET, int type = 0);

bool interceptkey(int sym);

struct Texture;

void renderbackground(const char *caption = NULL, Texture *mapshot = NULL, const char *mapname = NULL, const char *mapinfo = NULL, bool force = false);
void renderprogress(float bar, const char *text, bool background = false);

void getfps(int &fps, int &bestdiff, int &worstdiff);
int getclockmillis();

enum { KR_CONSOLE = 1<<0, KR_GUI = 1<<1, KR_EDITMODE = 1<<2 };

void keyrepeat(bool on, int mask = ~0);

enum { TI_CONSOLE = 1<<0, TI_GUI = 1<<1 };

void textinput(bool on, int mask = ~0);

void fatal(const char *s, ...) PRINTFARGS(1, 2);

bool isconnected(bool attempt = false, bool local = true);
bool multiplayer(bool msg = true);

#endif
