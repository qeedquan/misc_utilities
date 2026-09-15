#!/bin/sh

# Checkout everything (all branches from a repo)
git clone https://github.com/username/repository
cd repository
git fetch --all
for branch in $(git branch -r | grep -v '\->'); do
    git branch --track ${branch#origin/} $branch
done
git pull --all

