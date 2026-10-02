rm -rf build
mkdir build
cd build
cmake ..
make

export QT_QPA_PLATFORM=xcb
./qtgl
