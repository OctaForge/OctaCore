#ifndef ENGINE_MOVIE_HH
#define ENGINE_MOVIE_HH

namespace recorder
{
    void stop();
    void capture(bool overlay = true);
    void cleanup();
}

#endif
