#!/bin/bash

# Simple test of the command interpreter

# Test 1
in="abbabaq"
out="0,3,5;"
[[ $(./cmd_int <<< "$in") == "$out"* ]] && echo "Test 1 PASSED" || echo "Test 1 FAILED"

# Test 2
in="aabbcaaacbq"
out="0,1,2;"
[[ $(./cmd_int <<< "$in") == "$out"* ]] && echo "Test 2 PASSED" || echo "Test 2 FAILED"

# Test 3
in="abccbaabcq"
out="5;"
[[ $(./cmd_int <<< "$in") == "$out"* ]] && echo "Test 3 PASSED" || echo "Test 3 FAILED"

# Test 4
in="aaaaaqqq"
out="0,1,2,3,4;"
[[ $(./cmd_int <<< "$in") == "$out"* ]] && echo "Test 4 PASSED" || echo "Test 4 FAILED"
