#!/bin/bash

# Write a given string to a file, creating directories as needed.
# $0 <writefile> <writestr>
#   writefile: path to file to write
#   writestr : string to write to $writefile

# Written for CU Boulder ECEA 5305, Assignment 1

function usage() {
    prog="$(basename "$0")"
    echo
    echo "Write a given string to a file, creating directories as needed."
    echo "usage: $prog <writefile> <writestr>"
    echo
}

if [ $# -ne 2 ]
then
    echo "Improper number of arguments."
    usage
    exit 1
fi

writefile="$1"
writestr="$2"

mkdir -p "$(dirname "$writefile")" ||
    { echo Could not create directory $(dirname "$writefile").; exit 1; }
echo "$writestr" > "$writefile" ||
    { echo Could not create file "$writefile".; exit 1; }

exit 0
