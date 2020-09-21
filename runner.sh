#!/bin/bash

for var in "$@"
do
  echo "cmd: $var"
  case "$var" in
    "docker")
      export IMAGE_TAG=aml_tmp
      #export IMAGE_TAG_STAB=aml_stab
      export IMAGE_TAG_LATEST=aml_latest

      # docker pull $IMAGE_TAG_STAB   || true
      # docker pull $IMAGE_TAG_LATEST || true
      docker build --build-arg JOBS=4 --tag $IMAGE_TAG .
      # docker push $IMAGE_TAG

      # docker pull $IMAGE_TAG
      docker run --entrypoint aml_tests $IMAGE_TAG

      # docker pull $IMAGE_TAG
      docker tag  $IMAGE_TAG $IMAGE_TAG_LATEST
      # docker push $IMAGE_TAG_LATEST

      echo "deploy TODO"
      ;;

    "build")
      cmake -S. -B./build \
        && cmake --build ./build -j6 \
        && cmake --build ./build --target package \
        && cmake --build ./build --target test
      ;;

    "tests")
      ./build/aml_tests
      ;;

    "sample_compile")
      ./build/amlc \
      --cmd=compile \
      --file_input=aml/sample.aml \
      --file_output=logs/sample.binary \
      --log=logs/sample.compile.log \
      --level=trace
      ;;

    "sample_execute")
      ./build/amlc \
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
