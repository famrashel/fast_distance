# fast_distance

Python module for fast computation of edit distance between strings

## Dependencies:
- swig (wrapper for C program)
- If you have older version of swig, you may need to update the setup.py script.


## Installation

From this directory (__'-e'__ is optional for edit mode):  
> pip install [-e] .

## Usage

- Help:  
	> python fast_distance -h  

	or  
	> python fast_distance.py —help

- Check distance between two strings
	> python FastDistance.py string1 string2

- Check distance between two strings (as module)
	> from _fast_distance import fast_distance  
	> fast_distance('toto', 'tata')
 
## Uninstall
> pip uninstall fast_distance