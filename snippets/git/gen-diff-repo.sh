#!/bin/sh

# given two git repos, generate a list of files that changed from one against the other and then
# create a folder structure that mimics the git repo with the changed files

if [ "$#" -ne 4 ]; then
	echo "usage: <gitdir> <to> <branch1> <branch2>"
	exit 2
fi

git -C $1 checkout $3

for file in $(git -C $1 diff $3..$4 --name-only)
do
	from=$1/$file
	to=$2/$(dirname $file)
	mkdir -p $to
	cp -v $from $to
done
