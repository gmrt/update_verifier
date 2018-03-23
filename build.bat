@echo off

set C_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG"
set CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG /EHsc"

git submodule update --init --depth=1 boringssl
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE=%C_FLAGS_RELEASE% -DCMAKE_CXX_FLAGS_RELEASE=%CXX_FLAGS_RELEASE% ../
ninja
cd ..
