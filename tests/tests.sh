#!/bin/bash

files=("fileTest1.txt" "fileTest2.test" "fileTest3.any" "fileTest4.any")

count=1
# shellcheck disable=SC2128
# shellcheck disable=SC2068
for file in ${files[@]}; do
    echo "test $count. files: $tmp, $file"
    tmp="$file.copy"
    rm -rf "$tmp"
    cp "$file" "$tmp"
    ./a.out "$tmp" "$1"
    ./a.out "$tmp" "$2"
    cmp --silent "$file" "$tmp" || echo "files are different"
    rm -rf "$tmp"
    count=$(($count + 1))
done
