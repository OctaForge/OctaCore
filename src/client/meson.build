client_src = [
    '../sauerlib/tools.cc',
    '../sauerlib/encoding.cc',
    '../sauerlib/stream.cc',
    '../shared/geom.cc',
    '../shared/glemu.cc',
    '../shared/gzstream.cc',
    '../shared/stream.cc',
    '../shared/zip.cc',
    '../engine/aa.cc',
    '../engine/bih.cc',
    '../engine/blend.cc',
    '../engine/command.cc',
    '../engine/console.cc',
    '../engine/dynlight.cc',
    '../engine/grass.cc',
    '../engine/light.cc',
    '../engine/main.cc',
    '../engine/material.cc',
    '../engine/movie.cc',
    '../engine/normal.cc',
    '../engine/octa.cc',
    '../engine/octaedit.cc',
    '../engine/octarender.cc',
    '../engine/physics.cc',
    '../engine/pvs.cc',
    '../engine/rendergl.cc',
    '../engine/renderlights.cc',
    '../engine/rendermodel.cc',
    '../engine/renderparticles.cc',
    '../engine/rendersky.cc',
    '../engine/rendertext.cc',
    '../engine/renderva.cc',
    '../engine/shader.cc',
    '../engine/stain.cc',
    '../engine/texture.cc',
    '../engine/water.cc',
    '../engine/world.cc',
    '../engine/worldio.cc',
    '../game/game.cc',
]

threads_dep = dependency('threads')
sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('SDL2_image')
zlib_dep = dependency('zlib')
gl_dep = dependency('gl')
rt_dep = cxx.find_library('rt', required: false)

executable('native_client',
    client_src,
    dependencies: [
        threads_dep, sdl2_dep, sdl2_image_dep,
        zlib_dep, gl_dep, rt_dep
    ],
    include_directories: octacore_includes
)
