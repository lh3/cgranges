try:
	from setuptools import setup, Extension
except ImportError:
	from distutils.core import setup
	from distutils.extension import Extension

cmdclass = {}

try:
	from Cython.Build import build_ext
except ImportError: # without Cython
	sources = ['cgranges.c']
else: # with Cython
	sources = ['cgranges.c', 'python/cgranges.pyx']
	cmdclass['build_ext'] = build_ext

import sys, platform

sys.path.append('python')

include_dirs = ["."]

def readme():
	with open('python/README.rst') as f:
		return f.read()

setup(
	name = 'cgranges',
	version = '0.1',
	url = 'https://github.com/lh3/cgranges',
	description = 'Genomic interval overlap query for python',
	long_description = readme(),
	author = 'Heng Li',
	author_email = 'lh3@me.com',
	license = 'MIT',
	keywords = 'interval',
	ext_modules = [Extension('cgranges',
		sources = sources,
		depends = ['cgranges.h', 'khash.h', 'python/cgranges.pyx'],
		include_dirs = include_dirs)],
	classifiers = [
		'Development Status :: 4 - Beta',
		'License :: OSI Approved :: MIT License',
		'Operating System :: POSIX',
		'Programming Language :: C',
		'Programming Language :: Cython',
		'Programming Language :: Python :: 2.7',
		'Programming Language :: Python :: 3',
		'Intended Audience :: Science/Research',
		'Topic :: Scientific/Engineering :: Bio-Informatics'],
	cmdclass = cmdclass)
