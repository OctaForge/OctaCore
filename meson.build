project('octacore', ['cpp'],
    version: '0.0.1',
    default_options: [
        'buildtype=debugoptimized', 'cpp_std=c++17', 'warning_level=1'
    ],
    meson_version: '>=0.46'
)

bin_path = join_paths(meson.source_root(), 'bin_unix')

cxx = meson.get_compiler('cpp')

subdir('src')
