#! /bin/bash

script_dir="$(cd "$(dirname $0)"; pwd -P)"
mkdir -p "${script_dir}/build"
cd "${script_dir}/build"
cmake ..

