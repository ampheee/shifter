#!/bin/bash

files=("1.test" "2.test" "3.test" "4.test" "6.test")

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