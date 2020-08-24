#!/bin/sh

BLACK=black
BLACK_OPTIONS="-l 120"

if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
    # Travis only clones the PR branch and uses its HEAD commit as detached HEAD,
    # so it's problematic when we want an exact commit range for format checks.
    # We fetch upstream to ensure that we have the proper references to resolve.
    # Ideally we would use $TRAVIS_COMMIT_RANGE but it doesn't play well with PR
    # updates, as it only includes changes since the previous state of the PR.
    if [ -z "$(git remote | grep upstream)" ]; then
      git remote add upstream https://github.com/godotengine/godot \
          --no-tags -f -t $TRAVIS_BRANCH
    fi
    RANGE="upstream/$TRAVIS_BRANCH HEAD"
else
    # Test only the last commit, since $TRAVIS_COMMIT_RANGE wouldn't support
    # force pushes.
    RANGE=HEAD
fi

FILES=$(git diff-tree --no-commit-id --name-only -r $RANGE | grep -v thirdparty/| grep -E "(SConstruct|SCsub|\.py)$")
echo "Checking files:\n$FILES"

# create a random filename to store our generated patch
prefix="static-check-black"
suffix="$(date +%s)"
patch="/tmp/$prefix-$suffix.patch"

for file in $FILES; do
    "$BLACK" "$BLACK_OPTIONS" --diff "$file" | \
        sed -e "1s|--- |--- a/|" -e "2s|+++ |+++ b/|" >> "$patch"
done

# if no patch has been generated all is ok, clean up the file stub and exit
if [ ! -s "$patch" ] ; then
    printf "Files in this commit comply with the black formatting rules.\n"
    rm -f "$patch"
    exit 0
fi

# a patch has been created, notify the user and exit
printf "\n*** The following differences were found between the code to commit "
printf "and the black formatting rules:\n\n"
pygmentize -l diff "$patch"
printf "\n*** Aborting, please fix your commit(s) with 'git commit --amend' or 'git rebase -i <hash>'\n"
exit 1
