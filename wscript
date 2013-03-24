#! /usr/bin/env python2

import os.path
import sys
import types

build_tools = os.path.join('.', 'tools', 'waf')
sys.path.append(build_tools)

APPNAME = 'stechec2'
VERSION = '2012'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_cxx')
    opt.load('unittest_gtest')
    opt.load('boost')
    opt.load('ruby')

    opt.add_option('--enable-debug', action = 'store_true', default = False,
                   help = 'build a debug version', dest = 'debug')
    opt.add_option('--enable-werror', action = 'store_true', default = False,
                   help = 'interpret warnings as errors', dest = 'werror')

    opt.recurse('games')
    opt.recurse('tools')

def configure(conf):
    conf.load('compiler_cxx')
    conf.load('unittest_gtest')
    conf.load('ruby')

    # Look for Python 2
    try:
        conf.find_program('python2', var='PYTHON')
    except conf.errors.ConfigurationError:
        conf.find_program('python', var='PYTHON')

    # Warning flags
    conf.check_cxx(cxxflags = '-Wall')
    conf.check_cxx(cxxflags = '-Wextra')
    #conf.check_cxx(cxxflags = '-pedantic')

    conf.env.append_value('CXXFLAGS', ['-Wall', '-Wextra', #'-pedantic',
                                       '-Wno-variadic-macros'])

    # Check for C++0x
    conf.check_cxx(cxxflags = '-std=c++0x')
    conf.check_cxx(cxxflags = '-std=c++0x', header_name = "cstdint")
    conf.check_cxx(cxxflags = '-std=c++0x', msg = 'Checking for std::shared_ptr',
                   fragment = '''
                   #include <memory>
                   int main() { std::shared_ptr<int> p; }
                   ''')
    conf.check_cxx(cxxflags = '-std=c++0x', msg = 'Checking for std::array',
                   fragment = '''
                   #include <array>
                   int main() { std::array<int, 4> a; }
                   ''')
    conf.env.append_value('CXXFLAGS', '-std=c++0x')

    # Debug / Release
    if conf.options.debug:
        conf.check_cxx(cxxflags = '-g')
        conf.check_cxx(cxxflags = '-ggdb3')
        conf.env.append_value('DEFINES', '__DEBUG__')
        conf.env.append_value('CXXFLAGS', ['-g', '-ggdb3'])
    else:
        conf.check_cxx(cxxflags = '-O2')
        conf.check_cxx(cxxflags = '-ffast-math')
        conf.env.append_value('CXXFLAGS', ['-O2', '-ffast-math'])

    # ZeroMQ and C++ binding (cppzmq)
    conf.check_cfg(package = 'libzmq', uselib_store = 'ZeroMQ',
                   atleast_version = '3.2.0', args = ['--cflags', '--libs'])
    conf.check(header_name='zmq.hpp')

    # Google Flags
    conf.check_cxx(lib = "gflags", mandatory = True, uselib_store = "gflags")

    # Ruby
    conf.check_ruby_version((1, 9))

    # -lrt
    conf.check_cxx(lib = "rt", mandatory = True, uselib_store = "rt")

    # Werror support - at the end to avoid false negatives in the checks
    if conf.options.werror:
        conf.check_cxx(cxxflags = '-Werror')
        conf.env.append_value('CXXFLAGS', '-Werror')

    # Configure games
    conf.recurse('games')

    # Configure tools
    conf.recurse('tools')

def build(bld):
    build_libs(bld)
    build_client(bld)
    build_server(bld)

    bld.recurse('games')
    bld.recurse('tools')

def build_libs(bld):
    build_net(bld)
    build_rules(bld)
    build_utils(bld)

def build_net(bld):
    bld.shlib(
        source = '''
            src/lib/net/socket.cc
            src/lib/net/server-socket.cc
            src/lib/net/client-socket.cc
            src/lib/net/message.cc
            src/lib/net/signal.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_PURPLE', 'MODULE_NAME="network"'],
        target = 'stechec2-net',
        use = ['ZeroMQ', 'stechec2-utils'],
        export_includes = 'src/lib'
    )

    for test in []:
        bld.program(
            features = 'gtest',
            source = 'src/lib/net/tests/test-%s.cc' % test,
            target = 'utils-test-%s' % test,
            use = ['stechec2-net']
        )

def build_utils(bld):
    bld.shlib(
        source = '''
            src/lib/utils/dll.cc
            src/lib/utils/log.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_GREEN', 'MODULE_NAME="utils"'],
        target = 'stechec2-utils',
        use = ['rt', 'gflags'],
        lib = ['dl'],
        export_includes = 'src/lib'
    )

    for test in ['buffer', 'sandbox']:
        bld.program(
            features = 'gtest',
            source = 'src/lib/utils/tests/test-%s.cc' % test,
            target = 'utils-test-%s' % test,
            use = ['stechec2-utils']
        )

def build_rules(bld):
    bld.shlib(
        source = '''
            src/lib/rules/action.cc
            src/lib/rules/actions.cc
            src/lib/rules/game-state.cc
            src/lib/rules/messenger.cc
            src/lib/rules/client-messenger.cc
            src/lib/rules/server-messenger.cc
            src/lib/rules/rules.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_BLUE', 'MODULE_NAME="rules"'],
        target = 'stechec2-rules',
        use = ['stechec2-utils', 'stechec2-net'],
        export_includes = 'src/lib'
    )

    for test in ['action', 'state']:
        bld.program(
            features = 'gtest',
            source = 'src/lib/rules/tests/test-%s.cc' % test,
            target = 'rules-test-%s' % test,
            use = ['stechec2-rules']
        )

def build_client(bld):
    bld.program(
        source = '''
            src/client/main.cc
            src/client/client.cc
        ''',
        target = 'stechec2-client',
        defines = ['MODULE_COLOR=ANSI_COL_YELLOW', 'MODULE_NAME="client"',
            'MODULE_VERSION="%s"' % VERSION],
        use = ['stechec2-utils', 'stechec2-net', 'stechec2-rules', 'gflags']
    )

def build_server(bld):
    bld.program(
        source = '''
            src/server/main.cc
            src/server/server.cc
        ''',
        target = 'stechec2-server',
        defines = ['MODULE_COLOR=ANSI_COL_RED', 'MODULE_NAME="server"',
            'MODULE_VERSION="%s"' % VERSION],
        use = ['stechec2-utils', 'stechec2-net', 'stechec2-rules', 'gflags']
    )
