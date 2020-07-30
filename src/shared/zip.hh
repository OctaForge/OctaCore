#ifndef SHARED_ZIP_HH
#define SHARED_ZIP_HH

#include <sauerlib/stream.hh>
#include <sauerlib/tools.hh>

bool findzipfile(const char *filename);
stream *openzipfile(const char *filename, const char *mode);
int listzipfiles(const char *dir, const char *ext, vector<char *> &files);

#endif
