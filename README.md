# CS621Idli

Project Setup Instructions

# ns-3 Setup
1. sudo apt-get install mercurial gcc g++ git cmake 
2. sudo apt-get python-dev python-setuptools qt5-default
3. cd && mkdir workspace && cd ~/workspace
4. hg clone http://code.nsnam.org/ns-3-allinone
5. cd ns-3-allinone/
6. ./download.py -n ns-3-dev
7. ./build.py
8. cd ns-3-dev
9. ./waf --enable-tests configure
10. ./test.py

# Repo setup to ns-3
0. cd ~/workspace/
1. git clone https://github.com/anuragjha/CS621Idli.git
2. sudo apt-get install rsync
3. rsync -a ~/workspace/CS621Idli/ns-3-dev/ ~/workspace/ns-3-allinone/ns-3-dev/ 

# libdash setup
1. cd ~/workspace/ns-3-allinone
2. git clone https://github.com/ChristianKreuzberger/AMuSt-libdash.git
3. sudo apt-get install build-essential gccxml git-core cmake 
4. sudo apt-get install libxml2-dev libcurl4-openssl-dev  
5. sudo apt-get install  libxslt-dev python-dev lib32z1-dev
6. sudo apt-get install --no-install-recommends libboost-all-dev
7. cd ~/workspace/ns-3-allinone/AMuSt-libdash/libdash
8. mkdir build
9. cd build
10. cmake ../
11. make

# Running project 
1. cd ~/workspace/ns-3-allinone/ns-3-dev/
2. ./waf configure 
3. ./waf --run "cs621IdliP1 --cfFileName=config.txt"