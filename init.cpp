import sys
import argparse

import prob


def init():
	arser = argparse.ArgumentParser()
	parser.add_argument('-v', '--verbose', action='store_true')
	args = parser.parse_args()
	
	if args.verbose: logging.basicConfig(level=logging.DEBUG)




