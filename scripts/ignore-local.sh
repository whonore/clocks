#!/bin/sh
set -e

TOP=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)/..

find "$TOP" -type f -name local.h -exec git update-index --skip-worktree {} \;
