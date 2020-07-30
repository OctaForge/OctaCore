#ifndef ENGINE_COMMAND_HH
#define ENGINE_COMMAND_HH

#include <shared/tools.hh>
#include <shared/command.hh>

extern hashnameset<ident> idents;
extern int identflags;

void clearoverrides();
void writecfg(const char *name = nullptr);

void checksleep(int millis);
void clearsleep(bool clearoverrides = true);

#endif
