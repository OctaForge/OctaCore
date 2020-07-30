#ifndef SHARED_GZSTREAM_HH
#define SHARED_GZSTREAM_HH

#include <sauerlib/stream.hh>

stream *opengzfile(const char *filename, const char *mode, stream *file = nullptr);

#endif
