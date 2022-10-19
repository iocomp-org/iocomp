from utils import *
import argparse

"""
command line arguments 
"""
parser = argparse.ArgumentParser(description='Plotting script for benchmark')
parser.add_argument('path', action='store',type=str,help='filename')
parser.add_argument('--flag',action=argparse.BooleanOptionalAction,help='filename')
args = parser.parse_args()

"""
function calls
"""
barplot(args.path, args.flag)
