#!/bin/bash
# argument 1 : ABI (x86_64, x86, aarch64)
# argument 2 : Debug or Release
RC_BASE=$(pwd)
echo "Base directory $RC_BASE"
RC_SYSTEM_TYPE=Linux
RC_BUILD_TYPE=Debug
if [ $# -lt 4 ]
  then
    echo "invoke with arg1 arg2 arg3 arg4"
    echo "argument 1 (architecture)  : ABI (x86_64, x86, aarch64)"
    echo "argument 2 (target)        : x86_64, imx8, renesas, semidrive, telechips"
    echo "argument 3 (window system) : default, x11, wayland, drm"
    echo "argument 4 (build config)  : Debug or Release"
    RC_Linux=$RC_SYSTEM_TYPE/x86_64
    RC_ABI=x86_64
else
    RC_ABI=$1
    RC_Linux=$RC_SYSTEM_TYPE/$1
    if [ $1 = x86_64 ]
      then
        RC_Linux=$RC_SYSTEM_TYPE/$1
    elif [ $1 = x86 ]
      then
        RC_Linux=$RC_SYSTEM_TYPE/$1
    else
      RC_Linux=$RC_SYSTEM_TYPE/$1
    fi
    RC_BUILD_TYPE=$4
fi
if [ $3 = default ]
 then
    WINSYS="DEFDISP"
elif [ $3 = x11 ]
 then
    WINSYS="X11"
elif [ $3 = drm ]
 then
    WINSYS="DRM"
else
    WINSYS="WAYLAND"
fi
echo "Building Spark_demo_app $RC_BUILD_TYPE build, WINSYS=$WINSYS"
RC_BUILD=$RC_BASE/build
TC_FILE=$RC_BASE/linux_toolchain_$RC_ABI.txt
mkdir -p $RC_BASE/prebuilt/bin/$RC_Linux/$RC_BUILD_TYPE
mkdir -p $RC_BASE/build/$RC_Linux/$RC_BUILD_TYPE/spark_demo_app
cmake -B$RC_BASE/build/$RC_Linux/$RC_BUILD_TYPE/spark_demo_app -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$TC_FILE -DCMAKE_BUILD_TYPE=$RC_BUILD_TYPE -DRC_WINSYS=$WINSYS -DGLES_VERSION=30
cd $RC_BASE/build/$RC_Linux/$RC_BUILD_TYPE/spark_demo_app
make -j`nproc`
# cp spark_demo_app $RC_BASE/prebuilt/bin/$RC_Linux/$RC_BUILD_TYPE/.
if [[ $? -eq 0 ]]
 then
  echo "spark_demo_app done"
  exit 0
else
  echo "spark_demo_app failed"
  exit 1
fi
cd $RC_BASE
