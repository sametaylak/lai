#!/usr/bin/env bash

SOURCE=${BASH_SOURCE[0]}

$(dirname "$SOURCE")/premake/premake5_macosx --file=./premake5.lua xcode4