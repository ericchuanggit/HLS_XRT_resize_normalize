project('project', 'cpp')
target = 'main'

cc = meson.get_compiler('cpp')

dep = [
    dependency('opencv4', version : '>=4.4.0'), 
    dependency('xrt', version : '>= 2.6'), 
    dependency('OpenCL'), 
    dependency('uuid')
]

inc_dir = [
    '/home/eric/sdk_125/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/xrt/',
    '/home/eric/sdk_125/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/',
    'inc'
]


libs = [static_library('_hls_resize_normalize', 'src/hls_resize_normalize.cpp', dependencies : dep, include_directories : inc_dir)]

shared_library('_hls_resize_normalize', 'src/hls_resize_normalize.cpp', dependencies : dep, include_directories : inc_dir)

exe = executable('project', target + '.cpp', link_with : libs, dependencies : dep, include_directories : inc_dir)
