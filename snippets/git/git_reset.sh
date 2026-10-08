# Given a git repository, reset the history to initial state

git_reset() {
        if [ ! -d ".git" ]
        then
                echo "No git repo here"
                return
        fi
        URL=$(grep 'url' .git/config | cut -d '=' -f2)
        echo "Resetting $URL"
        rm -rf .git
        git init
        git add .
        git commit -m "initial"
        git remote add origin $URL
        git push -u --force origin master
}

git_reset
