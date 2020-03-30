#!/usr/bin/zsh

if [[ "$1" == "debug" ]]; then
  clang-6.0 -g src/*.c -Iinclude/ -DDEBUG "${@:2}"
elif [[ "$1" == "build" ]]; then
  clang-6.0 -Wall -Werror -g src/*.c -Iinclude/ "${@:2}"
elif [[ "$1" == "clean" ]]; then
  rm ./a.out
else
  echo "Option required"
fi
