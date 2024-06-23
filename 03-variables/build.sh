#!/usr/bin/env bash

export my_env=config
echo "starting with: ${my_env}"

cmake -S . -B build
echo "after configuration with: ${my_env}"

export my_env=pre_build
echo "before build with: ${my_env}"

cmake --build build
echo "after build with: ${my_env}"
