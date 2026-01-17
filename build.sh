#!/bin/bash


PROJECT_NAME=all
CMAKE_ARGLIST="\
    -DCMAKE_TOOLCHAIN_FILE=freestanding-toolchain.cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DCMAKE_COLOR_DIAGNOSTICS=ON
"

CMAKE_ORIGINAL_SCRIPT_PATH="$PWD" # Assuming that build.sh is in the same dir as the Root CMakeLists.txt
CMAKE_ROOT_BUILD_DIR="build"
CMAKE_INTRMD_BUILD_DIR=""
CLEAN_CURRENT_ROOT_BUILD_DIR=true
CONFIGURE_CMAKE_FLAG=false
BUILD_BINARIES_FLAG=false
RUN_BINARY_FLAG=false


if [ "$1" == "help" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    echo "Usage: $0 <build_type> <action>"
    echo ""
    echo "Arguments:"
    echo "  build_type   - Type of build: debug, release, release_dbginfo, debug_perf, release_perf"
    echo "  action       - Action to take: cleanbuild, configure, build, run"
    echo ""
    echo "Options:"
    echo "  help         - Display this help message"
    echo ""
    echo "Examples:"
    echo "  ./build.sh debug   build"
    echo "  ./build.sh release build"
    exit 0
fi




if [ $# -ne 2 ]; then
    echo "2 Arguments required to run the script"
    exit
fi


# argument parsing
if [ $1 = "debug" ];
then
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=Debug"
    CMAKE_INTRMD_BUILD_DIR+="debug/"

elif [ $1 = "debug_perf" ];
then
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=Debug"
    CMAKE_ARGLIST+=" -DMEASURE_PERFORMANCE_TIMEOUT=1"
    CMAKE_INTRMD_BUILD_DIR+="debug_perf/"

elif [ $1 = "release" ];
then
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=Release"
    CMAKE_INTRMD_BUILD_DIR+="release/"

elif [ $1 = "release_dbginfo" ];
then
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=RelWithDbgInfo"
    CMAKE_INTRMD_BUILD_DIR+="release_dbginfo/"

elif [ $1 = "release_perf" ];
then
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=Release"
    CMAKE_ARGLIST+=" -DMEASURE_PERFORMANCE_TIMEOUT=1"
    CMAKE_INTRMD_BUILD_DIR+="release_perf/"

else
    printf "Unknown Argument %s - valid values are: debug, release, release_dbginfo\nExiting..." $1
    exit
fi


if [ $2 = "cleanbuild" ];
then
    CLEAN_CURRENT_ROOT_BUILD_DIR=true
    CONFIGURE_CMAKE_FLAG=false
    BUILD_BINARIES_FLAG=false
    RUN_BINARY_FLAG=false

elif [ $2 = "configure" ];
then
    CLEAN_CURRENT_ROOT_BUILD_DIR=false
    CONFIGURE_CMAKE_FLAG=true
    BUILD_BINARIES_FLAG=false
    RUN_BINARY_FLAG=false
    CMAKE_ARGLIST+=" -DGIT_SUBMODULE=ON"

elif [ $2 = "build" ];
then
    CLEAN_CURRENT_ROOT_BUILD_DIR=false
    CONFIGURE_CMAKE_FLAG=false
    BUILD_BINARIES_FLAG=true
    RUN_BINARY_FLAG=false

elif [ $2 = "run" ];
then
    CLEAN_CURRENT_ROOT_BUILD_DIR=false
    CONFIGURE_CMAKE_FLAG=false
    BUILD_BINARIES_FLAG=false
    RUN_BINARY_FLAG=true
else
    printf "Unknown Argument %s - valid values are: cleanbuild, configure, build, run\nExiting..." $3
    exit
fi




# the actual script
CMAKE_FINAL_BUILD_DIR="$CMAKE_ROOT_BUILD_DIR/$CMAKE_INTRMD_BUILD_DIR"
echo "Out-of-source Root   Build Directory is '$CMAKE_ROOT_BUILD_DIR' "
echo "Out-of-source Target Build Directory is '$CMAKE_FINAL_BUILD_DIR' "
echo "Cmake Arguments passed are ==> { "
echo "$CMAKE_ARGLIST"
echo "}"
echo "Script arguments are '$1' '$2' "


mkdir -p build
if [ "$CLEAN_CURRENT_ROOT_BUILD_DIR" = "true" ];
then
    rm -rf $CMAKE_FINAL_BUILD_DIR
fi


if [ "$CONFIGURE_CMAKE_FLAG" = "true" ]
then
mkdir -p $CMAKE_FINAL_BUILD_DIR
cmake -S . -B $CMAKE_FINAL_BUILD_DIR -G 'Ninja' $CMAKE_ARGLIST
fi


if [ "$BUILD_BINARIES_FLAG" = "true" ];
then
cd $CMAKE_FINAL_BUILD_DIR
cp "compile_commands.json" "../../compile_commands.json" 
# echo "CURRENT WORKING DIRECTORY IS $PWD"
ninja $PROJECT_NAME
fi


if [ $RUN_BINARY_FLAG = "true" ];
then
    cd $CMAKE_FINAL_BUILD_DIR # This assumes we already built
    ninja run_binary
fi


# cd ../ # leave static/shared
# cd ../ # leave debug/release/etc...
# cd ../ # build