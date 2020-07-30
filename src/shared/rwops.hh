#ifndef SHARED_RWOPS_HH
#define SHARED_RWOPS_HH

#include <SDL.h>

#include "tools.hh"

SDL_RWops *stream_rwops(stream *s);

#endif
