client_src = [
    '../shared/crypto.cpp',
    '../shared/stream.cpp',
    '../shared/tools.cpp',
    '../engine/command.cpp',
    '../engine/master.cpp'
]

threads_dep = dependency('threads')
zlib_dep = dependency('zlib')

executable('native_master',
    client_src,
    dependencies: [
        threads_dep, libenet, zlib_dep
    ],
    include_directories: octacore_includes,
    cpp_args: ['-DSTANDALONE'],
    install: true,
    install_dir: join_paths(meson.source_root(), 'bin_unix')
)
