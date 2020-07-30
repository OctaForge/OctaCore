#ifndef SHARED_RWOPS_HH
#define SHARED_RWOPS_HH

#include <SDL.h>

#include "tools.hh"

namespace detail {
    inline Sint64 rwopsseek(SDL_RWops *rw, Sint64 pos, int whence) {
        stream *f = (stream *)rw->hidden.unknown.data1;
        if((!pos && whence==SEEK_CUR) || f->seek(pos, whence)) return (int)f->tell();
        return -1;
    }

    inline size_t rwopsread(SDL_RWops *rw, void *buf, size_t size, size_t nmemb) {
        stream *f = (stream *)rw->hidden.unknown.data1;
        return f->read(buf, size*nmemb)/size;
    }

    inline size_t rwopswrite(SDL_RWops *rw, const void *buf, size_t size, size_t nmemb) {
        stream *f = (stream *)rw->hidden.unknown.data1;
        return f->write(buf, size*nmemb)/size;
    }

    inline int rwopsclose(SDL_RWops *rw) {
        return 0;
    }
}

inline SDL_RWops *stream_rwops(stream *s) {
    SDL_RWops *rw = SDL_AllocRW();
    if(!rw) return nullptr;
    rw->hidden.unknown.data1 = s;
    rw->seek = detail::rwopsseek;
    rw->read = detail::rwopsread;
    rw->write = detail::rwopswrite;
    rw->close = detail::rwopsclose;
    return rw;
}

#endif
