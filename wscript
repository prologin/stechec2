#! /usr/bin/env python2

APPNAME = 'stechec2'
VERSION = '2012'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')
    conf.check(features = 'cxx cxxprogram',
               cxxflags = ['-std=c++0x', '-Wall', '-Wextra', '-pedantic',
                           '-Wformat=2', '-Winit-self', '-Wmissing-include-dirs',
                           '-Wfloat-equal', '-Wundef', '-Wredundant-decls'],
               includes = ['src/lib'],
               uselib_store = 'CXX')

def build(bld):
    bld.stlib(
        source = ' '.join([
            'src/lib/network/network.cc'
        ]),
        target = 'network',
        use = ['CXX']
    )

    bld.program(
        source = ' '.join([
            'src/client/client.cc',
        ]),
        target = 'client',
        use = ['CXX', 'network']
    )

    bld.program(
        source = ' '.join([
            'src/server/server.cc'
        ]),
        target = 'server',
        use = ['CXX', 'network']
    )
