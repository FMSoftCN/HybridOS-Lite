#!/bin/bash

if [ ! -f myconfig.sh ]; then
    cp config.sh myconfig.sh
fi

source myconfig.sh

build_libraries() {
    for comp in hicairo hisvg; do
        echo "BUILDING '$comp'"
        cd $comp
        ./autogen.sh && ./configure && make clean && make -j$NR_JOBS && sudo make install
        if [ "$?" != "0" ]; then
            echo "====="
            echo "ERROR WHEN COMPILING '$comp'"
            echo "====="
            exit 1
        fi
        cd ..
    done

    for comp in hibox hidomlayout gvfb; do
        echo "BUILDING '$comp'"
        cd $comp
        cmake . && make clean && make -j$NR_JOBS && sudo make install
        if [ "$?" != "0" ]; then
            echo "====="
            echo "ERROR WHEN COMPILING '$comp'"
            echo "====="
            exit 1
        fi
        cd ..
    done

    for comp in hibus; do
        echo "BUILDING '$comp'"
        cd $comp
        cmake $HIBUS_OPTS . && make clean && make -j$NR_JOBS && sudo make install
        if [ "$?" != "0" ]; then
            echo "====="
            echo "ERROR WHEN COMPILING '$comp'"
            echo "====="
            exit 1
        fi
        cd ..
    done
}


echo "INSTALL MiniGUI resource files"
cd minigui-res/
./autogen.sh && ./configure && make -j$NR_JOBS  && sudo make install
cd ..

echo "BUILDING MiniGUI CORE"
cd minigui/
./autogen.sh && ./configure --disable-static $MINIGUI_OPTS && make -j$NR_JOBS  && sudo make install
if [ "$?" != "0" ]; then
    echo "====="
    echo "ERROR WHEN COMPILING MiniGUI Core"
    echo "====="
    exit 1
fi
cd ..

echo "BUILDING MGEFF"
cd mgeff/
./autogen.sh && ./configure --disable-static $MGEFF_OPTS && make -j$NR_JOBS && sudo make install
if [ "$?" != "0" ]; then
    echo "====="
    echo "ERROR WHEN COMPILING MGEFF"
    echo "====="
    exit 1
fi
cd ..

build_libraries
