#!/usr/bin/env bash

SOURCE=${BASH_SOURCE[0]}

$(dirname "$SOURCE")/premake/premake5_macosx --file=./premake5.lua gmake2 && make config=debug