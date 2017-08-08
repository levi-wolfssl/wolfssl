#!/bin/dash

if ! [ -d "$1" ] || ! [ $# -gt 0 ]
then
    echo "Error: please supply a directory"
    exit 1
fi

# We'll need absolute paths, because we'll be cd'ing around a bunch
home=$(cd "$(dirname "$0")" && echo "/.${PWD%/}/")
home=${home%/}
out=$(cd "$1" && echo "/.${PWD%/}/")
out=${out%/}

report() {
    printf "  %-7s %s\n" "$1" "${2#$out/}"
}

for target in "$home"/*/target
do
    [ -f "$target" ] || continue

    target_dir=${target%/target}
    target_name=${target_dir#$home/}

    cd "$target_dir" || continue
    printf "%s: %s\n" "$(basename "$0")" "$target_name"

    case "$(file -b --mime-type "$target")" in
        text/x-shellscript) target_exe=$target_dir/.libs/target ;;
        *)                  target_exe=$target ;;
    esac

    cp "$target_exe" "$out/${target_name}_target"
    report "CP" "${target_name}_target"

    options=$target_dir/target.options
    if [ -f "$options" ]
    then
        cp "$target" "$out/${target_name}.options"
        report "CP" "${target_name}.options"
    fi

    corpus_zip=$out/${target_name}_seed_corpus.zip
    for corpus in "$target_dir"/*
    do
        [ -d "$corpus" ] || continue
        zip -q -r "$corpus_zip" "$corpus"
    done
    [ -e "$corpus_zip" ] && report "ZIP" "$corpus_zip"
done

for dictionary in "$home"/*.dict
do
    [ -f "$dictionary" ] || continue
    cp "$dictionary" "$out/${dictionary#$home/}"
    report "CP" "$dictionary"
done

exit 0
