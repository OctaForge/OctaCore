#ifndef GL_HH
#define GL_HH

#ifndef STANDALONE
#  ifdef __APPLE__
#    include "SDL2/SDL.h"
#    define GL_GLEXT_LEGACY
#    define __glext_h_
#    include <OpenGL/gl.h>
#    define main SDL_main
#  else
#    include <SDL.h>
#    include <SDL_opengl.h>
#  endif
#endif

#endif
