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

    # ZeroMQ
    conf.check_cfg(package = 'libzmq', uselib_store = 'ZeroMQ',
                   args = ['--cflags', '--libs'])

    # Boost
    conf.load('boost')
    conf.check_boost(lib = 'program_options')

    # Ruby
    conf.check_ruby_version((1, 9))

    # Werror support - at the end to avoid false negatives in the checks
    if conf.options.werror:
        conf.check_cxx(cxxflags = '-Werror')
        conf.env.append_value('CXXFLAGS', '-Werror')

    # Configure games
    conf.recurse('games')

    # Configure tools
    conf.recurse('tools')

def _build_internal_lib(bld, **kwargs):
    """Helper to build an internal library, which is static but can be linked
    with a shared library."""
    cxxflags = kwargs.get('cxxflags', [])
    cxxflags.extend(bld.env.CXXFLAGS_cxxshlib)
    kwargs['cxxflags'] = cxxflags
    bld.stlib(**kwargs)

def build(bld):
    # Add some extensions to the bld object
    bld.internal_lib = types.MethodType(_build_internal_lib, bld)

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
    bld.internal_lib(
        source = '''
            src/lib/net/socket.cc
            src/lib/net/server-socket.cc
            src/lib/net/client-socket.cc
            src/lib/net/message.cc
            src/lib/net/messenger.cc
            src/lib/net/client-messenger.cc
            src/lib/net/server-messenger.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_PURPLE', 'MODULE_NAME="network"'],
        target = 'net',
        use = ['ZeroMQ', 'utils'],
        export_includes = 'src/lib'
    )

    for test in ['messenger']:
        bld.program(
            features = 'gtest',
            source = 'src/lib/net/tests/test-%s.cc' % test,
            target = 'utils-test-%s' % test,
            use = ['net', 'BOOST']
        )

def build_utils(bld):
    bld.internal_lib(
        source = '''
            src/lib/utils/dll.cc
            src/lib/utils/log.cc
            src/lib/utils/options.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_GREEN', 'MODULE_NAME="utils"'],
        target = 'utils',
        export_includes = 'src/lib'
    )

    for test in ['buffer', 'options']:
        bld.program(
            features = 'gtest',
            source = 'src/lib/utils/tests/test-%s.cc' % test,
            target = 'utils-test-%s' % test,
            use = ['utils', 'BOOST']
        )

def build_rules(bld):
    bld.internal_lib(
        source = '''
            src/lib/rules/action.cc
            src/lib/rules/game-state.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_BLUE', 'MODULE_NAME="rules"'],
        target = 'rules',
        use = ['utils'],
        export_includes = 'src/lib'
    )

    for test in ['action', 'state']:
        bld.program(
            features = 'gtest',
            source = 'src/lib/rules/tests/test-%s.cc' % test,
            target = 'rules-test-%s' % test,
            use = ['rules']
        )

def build_client(bld):
    bld.program(
        source = '''
            src/client/main.cc
            src/client/options.cc
            src/client/client.cc
        ''',
        target = 'stechec2-client',
        defines = ['MODULE_COLOR=ANSI_COL_YELLOW', 'MODULE_NAME="client"',
            'MODULE_VERSION="%s"' % VERSION],
        use = ['utils', 'net', 'BOOST'],
        lib = ['dl']
    )

def build_server(bld):
    bld.program(
        source = '''
            src/server/main.cc
            src/server/options.cc
            src/server/server.cc
            src/server/client.cc
        ''',
        target = 'stechec2-server',
        defines = ['MODULE_COLOR=ANSI_COL_RED', 'MODULE_NAME="server"',
            'MODULE_VERSION="%s"' % VERSION],
        use = ['utils', 'net', 'BOOST'],
        lib = ['dl']
    )
