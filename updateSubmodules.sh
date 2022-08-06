#!/bin/bash
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
git -C $SCRIPT_DIR submodule update --merge --recursive --remote --init --force
