#!/usr/bin/env bash

# https://github.com/holmesjr/bash-spec-2
. tests/bash-spec.sh

function run {
    cd tests
    ../olambda $@ 2>&1 | xargs
}

it 'can be used as a scientific calculator' && {
    expect $(run 1+1) to_be '2'
}

it 'can manipulate images' && {
    expect $(run cameraman.tif 'x(1,1)') to_be '156'
}

# TODO: statements ending with a variable
# TODO: statements ending with an assignment
# TODO: statements ending with an expression

it 'outputs errors when something is wrong' && {
    r=$(run 'n')
    should_fail
    expect_var r to_be "error: n undefined near line 1 column 1"
}

