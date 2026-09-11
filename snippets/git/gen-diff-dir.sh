#!/bin/sh

# given a git repo, generate a list of files that changed and
# create a folder structure that mimics the git repo with the changed files

if [ "$#" -ne 2 ]; then
	echo "usage: <gitdir> <to>"
	exit 2
fi

for file in $(git -C $1 diff --name-only)
do
	from=$1/$file
	to=$2/$(dirname $file)
	mkdir -p $to
	cp -v $from $to
done
