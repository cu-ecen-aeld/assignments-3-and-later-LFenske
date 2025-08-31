#!/bin/sh

# Search a directory recursively for a given string.
# Silently skip files and directories that we can't read.
# $0 <filesdir> <searchstr>
#   filesdir : directory to search
#   searchstr: string to search for

# Written for CU Boulder ECEA 5305, Assignment 1

function usage() {
    prog="$(basename "$0")"
    echo
    echo "Search a directory recursively for a given string."
    echo "usage: $prog <filesdir> <searchstr>"
    echo
}

if [ $# -ne 2 ]
then
    echo "Improper number of arguments."
    usage
    exit 1
fi

filesdir="$1"
searchstr="$2"

if [ ! -d "$filesdir" ]
then
    echo "Directory does not exist: $filesdir"
    usage
    exit 1
fi

numfiles=$(find "$filesdir" -type f        2>/dev/null | wc -l)
matches=$(grep -r "$searchstr" "$filesdir" 2>/dev/null | wc -l)

# Grammar error: "are" should be "is".
# Leave this as is so that unit tests work.
echo The number of files are $numfiles and the number of matching lines are $matches

exit 0
