# -*- coding: iso-8859-1 -*-
"""
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
"""

import os, sys, string
from distutils.cmd import Command
from distutils.command.build_ext import build_ext
from distutils.command.install_data import install_data
from distutils.command.install import install
from distutils.file_util import copy_file
from distutils.ccompiler import get_default_compiler
from distutils.core import setup, Extension
from distutils import sysconfig

class test(Command):
    # Original version of this class posted
    # by Berthold H�llmann to distutils-sig@python.org
    description = "test the distribution prior to install"

    user_options = [
        ('test-dir=', None,
         "directory that contains the test definitions"),
        ]

    def initialize_options(self):
        self.build_base = 'build'
        self.test_dir = 'test'

    def finalize_options(self):
        build = self.get_finalized_command('build')
        self.build_purelib = build.build_purelib
        self.build_platlib = build.build_platlib

    def run(self):
        # Testing depends on the module having been built
        self.run_command('build')

        # extend sys.path
        old_path = sys.path[:]
        sys.path.insert(0, self.build_purelib)
        sys.path.insert(0, self.build_platlib)
        sys.path.insert(0, self.test_dir)

        # import and run test-suite
        module = __import__('QuantLibTestSuite', globals(), locals(), [''])
        module.test()

        # restore sys.path
        sys.path = old_path[:]

class my_wrap(Command):
    description = "generate Python wrappers"
    user_options = []
    def initialize_options(self): pass
    def finalize_options(self): pass
    def run(self):
        print 'Generating Python bindings for QuantLib...'
        swig_dir = os.path.join("..","SWIG")
        os.system('swig -python -c++ -modern ' +
                  '-I%s ' % swig_dir +
                  '-outdir QuantLib -o QuantLib/quantlib_wrap.cpp ' +
                  'quantlib.i')

class my_build_ext(build_ext):
    user_options = build_ext.user_options + [
        ('dll', None,
         "link against CRTDLL libraries on Windows")
    ]
    def initialize_options(self):
        build_ext.initialize_options(self)
        self.use_dll = None
    def finalize_options(self):
        build_ext.finalize_options(self)

        self.include_dirs = self.include_dirs or []
        self.library_dirs = self.library_dirs or []
        self.define = self.define or []
        self.libraries = self.libraries or []

        extra_compile_args = []
        extra_link_args = []

        compiler = self.compiler or get_default_compiler()

        if compiler == 'msvc':
            print 'compiler =', 'MSVC'
            try:
                QL_INSTALL_DIR = os.environ['QL_DIR']
                self.include_dirs += [QL_INSTALL_DIR]
                self.library_dirs += [os.path.join(QL_INSTALL_DIR, 'lib')]
            except KeyError, e:
                print 'warning: unable to detect QuantLib installation'

            if os.environ.has_key('INCLUDE'):
                dirs = [dir for dir in os.environ['INCLUDE'].split(';')]
                self.include_dirs += dirs
            if os.environ.has_key('LIB'):
                dirs = [dir for dir in os.environ['LIB'].split(';')]
                self.library_dirs += dirs

            self.define += [('__WIN32__', None), ('WIN32', None),
                            ('NDEBUG', None), ('_WINDOWS', None),
                            ('NOMINMAX', None)]
            extra_compile_args = ['/GR', '/FD', '/Zm250']
            extra_link_args = ['/subsystem:windows', '/machine:I386']

            if self.debug:
                if self.use_dll:
                    extra_compile_args.append('/MDd')
                else:
                    extra_compile_args.append('/MTd')
            else:
                if self.use_dll:
                    extra_compile_args.append('/MD')
                else:
                    extra_compile_args.append('/MT')

        elif compiler == 'unix':
            print 'compiler =', 'Unix'
            ql_compile_args = \
                os.popen('quantlib-config --cflags').read()[:-1].split()
            ql_link_args = \
                os.popen('quantlib-config --libs').read()[:-1].split()
            self.define += [ (arg[2:],None) for arg in ql_compile_args
                             if arg.startswith('-D') ]
            self.include_dirs += [ arg[2:] for arg in ql_compile_args
                                   if arg.startswith('-I') ]
            self.library_dirs += [ arg[2:] for arg in ql_link_args
                                   if arg.startswith('-L') ]
            self.libraries += [ arg[2:] for arg in ql_link_args
                                if arg.startswith('-l') ]

            extra_compile_args = [ arg for arg in ql_compile_args
                                   if not arg.startswith('-D')
                                   if not arg.startswith('-I') ] \
                                   + [ '-Wno-unused' ]
            if os.environ.has_key('CXXFLAGS'):
                extra_compile_args += string.split(os.environ['CXXFLAGS'])

            extra_link_args = [ arg for arg in ql_link_args
                                if not arg.startswith('-L')
                                if not arg.startswith('-l') ]

        else:
            print 'compiler =', self.compiler
            pass

        for ext in self.extensions:
            ext.extra_compile_args = ext.extra_compile_args or []
            ext.extra_compile_args += extra_compile_args

            ext.extra_link_args = ext.extra_link_args or []
            ext.extra_link_args += extra_link_args

if os.name == 'posix':
    # changes the compiler from gcc to g++
    save_init_posix = sysconfig._init_posix
    def my_init_posix():
        save_init_posix()
        g = sysconfig._config_vars
        if os.environ.has_key('CXX'):
            g['CC'] = os.environ['CXX']
        else:
            g['CC'] = 'g++'
        if sys.platform.startswith("darwin"):
            g['LDSHARED'] = g['CC'] + \
                            ' -bundle -flat_namespace -undefined suppress'
        else:
            g['LDSHARED'] = g['CC'] + ' -shared'
    sysconfig._init_posix = my_init_posix

datafiles  = []

# patch distutils if it can't cope with the "classifiers" or
# "download_url" keywords
if sys.version < '2.2.3':
    from distutils.dist import DistributionMetadata
    DistributionMetadata.classifiers = None
    DistributionMetadata.download_url = None

classifiers = [
    'Development Status :: 4 - Beta',
    'Environment :: Console',
    'Intended Audience :: Developers',
    'Intended Audience :: End Users/Desktop',
    'License :: OSI Approved :: BSD License',
    'Natural Language :: English',
    'Operating System :: OS Independent',
    'Programming Language :: Python',
    'Topic :: Scientific/Engineering',
]

setup(name             = "QuantLib-Python",
      version          = "0.9.0",
      description      = "Python bindings for the QuantLib library",
      long_description = """
QuantLib (http://quantlib.org/) is a C++ library for financial quantitative
analysts and developers, aimed at providing a comprehensive software
framework for quantitative finance.
      """,
      author           = "QuantLib Team",
      author_email     = "quantlib-users@lists.sourceforge.net",
      url              = "http://quantlib.org",
      license          = open('../LICENSE.TXT','r+').read(),
      classifiers      = classifiers,
      py_modules       = ['QuantLib.__init__','QuantLib.QuantLib'],
      ext_modules      = [Extension("QuantLib._QuantLib",
                                    ["QuantLib/quantlib_wrap.cpp"])
                         ],
      data_files       = datafiles,
      cmdclass         = {'test': test,
                          'wrap': my_wrap,
                          'build_ext': my_build_ext
                          }
      )

