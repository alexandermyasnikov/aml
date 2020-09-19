#!/bin/bash

for var in "$@"
do
  echo "cmd: $var"
  case "$var" in
    "build")
      cmake -S. -B./build \
        && cmake --build ./build -j6 \
        && cmake --build ./build --target test
      ;;

    "tests")
      ./build/aml_tests
      ;;

    "sample_compile")
      ./build/aml \
      --cmd=compile \
      --file_input=aml/sample.aml \
      --file_output=logs/sample.binary \
      --log=logs/sample.compile.log \
      --level=trace
      ;;

    "sample_execute")
      ./build/aml \
      --cmd=execute \
      --file_input=logs/sample.binary \
      --file_output=logs/sample.result \
      --log=logs/sample.execute.log \
      --level=trace \
      && cat logs/sample.result;
      ;;

    *)
      echo "ERROR: unknown cmd $var"
      ;;
  esac

  RC=$?
  echo ""
  date;
  echo "code: $RC"

  if [ $RC -ne 0 ]; then
    exit $RC
  fi

done
