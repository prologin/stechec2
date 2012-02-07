#! /usr/bin/env python2

APPNAME = 'stechec2'
VERSION = '2012'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_cxx')

    opt.add_option('--enable-debug', action='store_true', default=False,
                   help='build a debug version of the project', dest='debug')
    opt.add_option('--enable-werror', action='store_true', default=False,
                   help='interpret warnings as errors', dest='werror')

def configure(conf):
    conf.load('compiler_cxx')

    # Warning flags
    conf.check_cxx(cxxflags = '-Wall')
    conf.check_cxx(cxxflags = '-Wextra')
    conf.check_cxx(cxxflags = '-pedantic')

    conf.env.append_value('CXXFLAGS', ['-Wall', '-Wextra', '-pedantic'])

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

    # Werror support - at the end to avoid false negatives in the checks
    if conf.options.werror:
        conf.check_cxx(cxxflags = '-Werror')
        conf.env.append_value('CXXFLAGS', '-Werror')

def build(bld):
    bld.stlib(
        source = """
            src/lib/network/network.cc
        """,
        target = 'network'
    )

    bld.stlib(
        source = """
            src/lib/rules/rules.cc
        """,
        target = 'rules'
    )

    bld.program(
        source = """
            src/client/client.cc
        """,
        target = 'client',
        use = ['network']
    )

    bld.program(
        source = """
            src/server/server.cc
        """,
        target = 'server',
        use = ['network']
    )
