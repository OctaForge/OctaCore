#ifndef ENGINE_AA_HH
#define ENGINE_AA_HH

#include <shared/gl.hh>

#include <shared/geom.hh>

enum { AA_UNUSED = 0, AA_LUMA, AA_MASKED, AA_SPLIT, AA_SPLIT_LUMA, AA_SPLIT_MASKED };

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
