#ifndef ENGINE_CONSOLE_HH
#define ENGINE_CONSOLE_HH

#include <sauerlib/stream.hh>

struct tagval; /* command.hh */
struct ident;

void processkey(int code, bool isdown, int modstate = 0);
void processtextinput(const char *str, int len);
float rendercommand(float x, float y, float w);
float renderfullconsole(float w, float h);
float renderconsole(float w, float h, float abovehud);
const char *addreleaseaction(char *s);
tagval *addreleaseaction(ident *id, int numargs);
void writebinds(stream *f);
void writecompletions(stream *f);

enum
{
    CON_INFO  = 1<<0,
    CON_WARN  = 1<<1,
    CON_ERROR = 1<<2,
    CON_DEBUG = 1<<3,
    CON_INIT  = 1<<4,
    CON_ECHO  = 1<<5
};

void conoutf(const char *s, ...) PRINTFARGS(1, 2);
void conoutf(int type, const char *s, ...) PRINTFARGS(2, 3);
void conoutfv(int type, const char *fmt, va_list args);

#endif
