rm -rf build
mkdir build
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=/home/wangxt/workspace/projects/flowengine/platforms/linux/aarch64.jetson.cmake \
      -DTOOLCHAIN_ROOTDIR=/usr/bin .. 
# make -j