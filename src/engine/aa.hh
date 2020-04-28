#ifndef ENGINE_AA_HH
#define ENGINE_AA_HH

#include <shared/gl.hh>

#include <shared/geom.hh>

extern matrix4 nojittermatrix;

void setaavelocityparams(GLenum tmu = GL_TEXTURE0);

void setupaa(int w, int h);
void jitteraa();
void doaa(GLuint outfbo, void (*resolve)(GLuint, int));
bool debugaa();
void cleanupaa();

void setaamask(bool val);
void enableaamask(int stencil = 0);
void disableaamask();

bool maskedaa();
bool multisampledaa();

#endif
