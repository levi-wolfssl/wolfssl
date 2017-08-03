#!/bin/sh

if ! [ -d "$1" ] || ! [ $# -gt 0 ]
then
    echo "Error: please supply a directory"
    exit 1
fi

home=$(dirname "$0")
out=${1%/}

echo "to: $out"

for target_out in "$home"/*/target
do
    target=${target_out%/target}
    echo "$target"
done

exit 0
