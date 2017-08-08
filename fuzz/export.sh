#!/bin/sh

# enforce calling convention
if [ ! -d "$1" ] || [ $# -eq 0 ]
then
    echo "Error: please supply a directory." >&2
    exit 1
elif [ $# -ne 1 ]
then
    echo "Error: only one directory, please." >&2
    exit 1
fi

# We'll need absolute paths, because we'll be cd'ing around a bunch
### NOTE ######################################################################
# The below is a bit funny, I know, but it's the most robust and portable way
# that I know of to get absolute paths. Basically, if cd fails for some reason,
# the expansion will come up empty. Otherwise, the absolute path will always
# end with a slash, which is needed because otherwise if for some insane reason
# the last directory in $PWD ends with new line characters ('\n'), then the
# command substitution would strip them off. I then strip the trailing slash so
# that I can later use the variable in a natural fashion.
#
# Note that I insist on removing any slash from the end of $PWD. In all, it
# prevents doubling up on slashes, and it seems that a slash is forbidden from
# being part of a file name due to how the linux kernal itself handles file
# paths. As such, taking a single trailing slash off the end should never be a
# problem.
#
home=$(cd "$(dirname "$0")" && echo "${PWD%/}/")
[ -z "$home" ] && exit 255
home=${home%/}

out=$(cd "$1" && echo "${PWD%/}/")
[ -z "$out" ] && exit 255
out=${out%/}

# just a quick wrapper for formatting output
report() {
    printf "  %-7s %s\n" "$1" "$2"
}

for target in "$home"/*/target
do
    [ -f "$target" ] || continue

    target_dir=${target%/target}
    target_name=${target_dir#$home/}

    cd "$target_dir" || continue
    printf "%s: %s\n" "$(basename "$0")" "${target_name}_target"

    ### Note ##################################################################
    # We have to make sure we don't pick up the libtool script instead of the
    # proper executable, so we're going to check 'target' and '.libs/target'.
    # As best as I can tell, the command 'file' is a rather portable, so we'll
    # use it to evaluate what kind of file each one is. We'll ask for a mime
    # type rather than the default, more human readable output. Hopefully this
    # is well defined.
    #
    fuzz_mime=$(file -b --mime-type "$target")
    if [ -f "$target_dir"/.libs/target ]
    then
        libs_mime=$(file -b --mime-type "$target_dir/.libs/target")
    else
        libs_mime=
    fi

    # learn the executable mime-type from /bin/sh (-L to dereference links)
    exec_mime=$(file -b -L --mime-type /bin/sh)

    if [ "$fuzz_mime" = "$exec_mime" ]
    then
        target_exe=$target
    elif  [ "$libs_mime" = "$exec_mime" ]
    then
        target_exe=$target_dir/.libs/target
    else
        echo "ERROR: no binary executable found in the usual locations!" >&2
        continue
    fi

    # export the executable to the $out directory
    cp "$target_exe" "$out/${target_name}_target"
    report "CP" "${target_name}_target"

    options=$target_dir/target.options
    if [ -f "$options" ]
    then
        # export the options file to the $out directory
        cp "$options" "$out/${target_name}.options"
        report "CP" "${target_name}.options"
    fi

    corpus_zip=$out/${target_name}_seed_corpus.zip
    for corpus in "$target_dir"/*
    do
        # zip up any corpora
        ### Note ##############################################################
        # It looks like the official POSIX stance of globbing is that the * in
        # the above pattern won't match a name starting with a period, meaning
        # that it should not decend any hidden directories, which is desirable.
        # The intended assumption is that any non-hidden directory is a corpus.
        #
        [ -d "$corpus" ] || continue
        zip -q -r "$corpus_zip" "$corpus"
    done
    [ -e "$corpus_zip" ] && report "ZIP" "${corpus_zip#$out/}"
done

# @TODO: maybe have a dictionary directory rather than scattering them around
# the fuzz directory.
for dictionary in "$home"/*.dict
do
    [ -f "$dictionary" ] || continue

    # export the dictionary to $out directory, no name change needed!
    cp "$dictionary" "$out"
    report "CP" "$dictionary"
done

exit 0
