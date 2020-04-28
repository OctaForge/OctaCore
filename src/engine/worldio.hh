#ifndef ENGINE_WORLDIO_HH
#define ENGINE_WORLDIO_HH

uint getmapcrc();
void clearmapcrc();
bool load_world(const char *mname, const char *cname = NULL);

#endif
