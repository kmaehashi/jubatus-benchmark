# -*- mode: python; coding: utf-8 -*-

import glob

subdirs = "classifier"

def options(opt):
    opt.load('compiler_cxx')
    opt.add_option('--enable-glog',
                   action='store_true',
                   default=False,
                   help='enable google-glog for Bottleneck Benchmark')

    opt.recurse(subdirs)

def configure(conf):
    conf.env.CXXFLAGS += ['-O2', '-Wall', '-g' ]

    conf.load('compiler_cxx')

    # conf.check_cfg(package = 'jubatus', args = '--cflags --libs')
    conf.check_cfg(package = 'jubatus-client', args = '--cflags --libs')
    conf.check_cfg(package = 'pficommon', args = '--cflags --libs')
    conf.check_cxx(lib = 'rt', mandatory = True )

    if conf.options.enable_glog:
        conf.check_cfg(package = 'libglog', args = '--cflags --libs')
        conf.env.LINKFLAGS += ['-lglog']

    conf.recurse(subdirs)

def build(bld):
    bld.recurse(subdirs)
