#!/usr/bin/python
# -*- coding: utf-8 -*-

from setuptools import setup, Extension
import setuptools

setup(name = 'fast_distance',
	version = '4.2',
	description = 'fast computation of distance and similarity between strings',
	author = 'Yves Lepage & Rashel Fam',
	author_email = 'yves.lepage@waseda.jp & fam.rashel@fuji.waseda.jp',
	ext_modules =	[Extension('_fast_distance',
						sources = ['fast_distance_in_C/C/fast_distance.i', 'fast_distance_in_C/C/fast_distance.c', 'fast_distance_in_C/C/utf8.c'],
						# swig_opts=['-modern', '-new_repr'], # Comment this line if you have newer version of swig
						extra_compile_args = ['-std=c99']
					)],
	packages = setuptools.find_packages(),
	# python_requires = '>=3.5'
	)