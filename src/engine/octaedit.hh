#ifndef ENGINE_OCTAEDIT_HH
#define ENGINE_OCTAEDIT_HH

#include <shared/geom.hh>

#include "octa.hh"

enum { EDIT_FACE = 0, EDIT_TEX, EDIT_MAT, EDIT_FLIP, EDIT_COPY, EDIT_PASTE, EDIT_ROTATE, EDIT_REPLACE, EDIT_DELCUBE, EDIT_CALCLIGHT, EDIT_REMIP, EDIT_VSLOT, EDIT_UNDO, EDIT_REDO };

struct selinfo;
struct editinfo;
extern editinfo *localedit;
extern bool editmode;

extern vector<ushort> texmru;

void compacteditvslots();
void compactmruvslots();

void cancelsel();
void rendertexturepanel(int w, int h);
void addundo(undoblock *u);
void commitchanges(bool force = false);
void changed(const ivec &bbmin, const ivec &bbmax, bool commit = true);
void changed(const block3 &sel, bool commit = true);
void rendereditcursor();
void tryedit();

void renderprefab(const char *name, const vec &o, float yaw, float pitch, float roll, float size = 1, const vec &color = vec(1, 1, 1));
void previewprefab(const char *name, const vec &color);
void cleanupprefabs();

int shouldpacktex(int index);
bool packeditinfo(editinfo *e, int &inlen, uchar *&outbuf, int &outlen);
bool unpackeditinfo(editinfo *&e, const uchar *inbuf, int inlen, int outlen);
void freeeditinfo(editinfo *&e);
void pruneundos(int maxremain = 0);
bool packundo(int op, int &inlen, uchar *&outbuf, int &outlen);
bool unpackundo(const uchar *inbuf, int inlen, int outlen);
bool noedit(bool view = false, bool msg = true);
void toggleedit(bool force = true);
void mpeditface(int dir, int mode, selinfo &sel, bool local);
void mpedittex(int tex, int allfaces, selinfo &sel, bool local);
bool mpedittex(int tex, int allfaces, selinfo &sel, ucharbuf &buf);
void mpeditmat(int matid, int filter, selinfo &sel, bool local);
void mpflip(selinfo &sel, bool local);
void mpcopy(editinfo *&e, selinfo &sel, bool local);
void mppaste(editinfo *&e, selinfo &sel, bool local);
void mprotate(int cw, selinfo &sel, bool local);
void mpreplacetex(int oldtex, int newtex, bool insel, selinfo &sel, bool local);
bool mpreplacetex(int oldtex, int newtex, bool insel, selinfo &sel, ucharbuf &buf);
void mpdelcube(selinfo &sel, bool local);
void mpremip(bool local); // octa.cc
bool mpeditvslot(int delta, int allfaces, selinfo &sel, ucharbuf &buf);
void mpcalclight(bool local);

#endif
