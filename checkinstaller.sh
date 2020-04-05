#!/bin/bash
PREFIX=/tmp/test_install
./autogen.sh || exit 1
./configure --prefix=$PREFIX || exit 1
make install PREFIX=$PREFIX || exit 1

pushd $PREFIX
    mkdir -p src
    cat << EOF > ./src/libgps_check.c
#include <gpsdata.h>
#include <gpsutils.h>

int main()
{
    printf("This works: %s\n", gpsdata_antenna_tostring(GPSDATA_ANTENNA_ACTIVE));
    return 0;
}
EOF
    echo
    echo "Compiling code"
    echo
    gcc -o ./bin/libgps_check.out -I ./include -I ./src \
        ./src/libgps_check.c \
        -L ./lib -lgps_mtk3339 -Wl,-rpath $PREFIX/lib || exit 1
    echo "checking library links using ldd"
    echo
    ldd -d -r ./bin/libgps_check.out || exit 1
    echo
    echo "testing code function runs"
    echo
    ./bin/libgps_check.out || exit 1
    echo
popd
