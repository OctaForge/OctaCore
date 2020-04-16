client_src = [
    '../shared/crypto.cpp',
    '../shared/geom.cpp',
    '../shared/glemu.cpp',
    '../shared/stream.cpp',
    '../shared/tools.cpp',
    '../shared/zip.cpp',
    '../engine/aa.cpp',
    '../engine/bih.cpp',
    '../engine/blend.cpp',
    '../engine/client.cpp',
    '../engine/command.cpp',
    '../engine/console.cpp',
    '../engine/dynlight.cpp',
    '../engine/grass.cpp',
    '../engine/light.cpp',
    '../engine/main.cpp',
    '../engine/material.cpp',
    '../engine/menus.cpp',
    '../engine/movie.cpp',
    '../engine/normal.cpp',
    '../engine/octa.cpp',
    '../engine/octaedit.cpp',
    '../engine/octarender.cpp',
    '../engine/physics.cpp',
    '../engine/pvs.cpp',
    '../engine/rendergl.cpp',
    '../engine/renderlights.cpp',
    '../engine/rendermodel.cpp',
    '../engine/renderparticles.cpp',
    '../engine/rendersky.cpp',
    '../engine/rendertext.cpp',
    '../engine/renderva.cpp',
    '../engine/server.cpp',
    '../engine/serverbrowser.cpp',
    '../engine/shader.cpp',
    '../engine/stain.cpp',
    '../engine/texture.cpp',
    '../engine/ui.cpp',
    '../engine/water.cpp',
    '../engine/world.cpp',
    '../engine/worldio.cpp',
    '../game/ai.cpp',
    '../game/client.cpp',
    '../game/entities.cpp',
    '../game/game.cpp',
    '../game/render.cpp',
    '../game/scoreboard.cpp',
    '../game/server.cpp',
    '../game/waypoint.cpp',
    '../game/weapon.cpp'
]

threads_dep = dependency('threads')
sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('SDL2_image')
zlib_dep = dependency('zlib')
gl_dep = dependency('gl')
rt_dep = cc.find_library('rt', required: false)

executable('native_client',
    client_src,
    dependencies: [
        threads_dep, libenet, sdl2_dep, sdl2_image_dep,
        zlib_dep, gl_dep, rt_dep
    ],
    include_directories: octacore_includes,
    install: true,
    install_dir: join_paths(meson.source_root(), 'bin_unix')
)
