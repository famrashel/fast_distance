#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import time

from argparse import ArgumentParser, ArgumentTypeError

from _fast_distance import fast_distance, fast_similitude, init_memo_fast_distance, memo_fast_distance, memo_fast_similitude

###############################################################################

__author__				= 'Yves Lepage <yves.lepage@waseda.jp>, Rashel Fam <fam.rashel@fuji.waseda.jp>'
__date__, __version__	= '05/02/2016', '1.0'	# Creation.
__date__, __version__	= '17/05/2021', '1.1'	# Update to python3 - [FAM Rashel]
__description__			= 'Fast computation of distance and similarity between strings.'

###############################################################################

def read_argv():
	this_version = 'v%s (c) %s %s' % (__version__, __date__.split('/')[2], __author__)
	this_description = __description__
	this_usage = '''%(prog)s  stringA stringB
	'''

	parser = ArgumentParser(description=this_description, usage=this_usage, epilog=this_version)
	parser.add_argument('strings',
				  metavar='N', type=str, nargs=2,
				  help='2 strings to be computed')
	parser.add_argument('-V', '--verbose',
                  action='store_true', dest='verbose', default=False,
                  help='runs in verbose mode')

	return parser.parse_args()

###############################################################################

if __name__ == '__main__':
	options = read_argv()
	t1 = time.time()
	print(fast_distance(options.strings[0], options.strings[1]))
	if options.verbose:
		print(f'# Processing time: {(time.time() - t1):.2f}s', file=sys.stderr)
