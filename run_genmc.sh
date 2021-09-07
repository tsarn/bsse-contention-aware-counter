#!/bin/bash

pushd "$(dirname "$0")"

if [[ -z "$GENMC" ]]; then
    GENMC="docker run --rm -it -v $PWD:/root genmc/genmc genmc"
fi

$GENMC "./test.c"
