octacore_includes = [
    include_directories('shared', 'engine', 'game', 'enet/include')
]

# FIXME: remove, for now without this the engine behaves wonky
add_global_arguments('-fsigned-char', language: 'cpp')

subdir('enet')
subdir('client')
subdir('server')
subdir('master')
