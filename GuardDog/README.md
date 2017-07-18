# Pre-requisites

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git gnuplot cmake
```

# Install OpenCV

Download OpenCV from: https://sourceforge.net/projects/opencvlibrary/

Extract the zip into /home/pi/

```
sudo apt-get install build-essential pkg-config libgtk2.0-dev python-dev python-numpy
mkdir opencv_release
cd opencv_release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_ZLIB=ON -D BUILD_PYTHON_SUPPORT=ON ../opencv[TAB]
make
sudo make install
```

See full instructions here: https://indranilsinharoy.com/2012/11/01/installing-opencv-on-linux/

# Install raspicam

See the project's own README.

```
git clone git@github.com:cedricve/raspicam.git
cd raspicam
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

# Build GuardDog

```
mkdir build
cd build
cmake ..
make
```

