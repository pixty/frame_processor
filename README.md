# frame_processor

## Libs Installation
### OpenCV 3.2+
Clone opencv and opencv contrib:
```
git clone git@github.com:opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib
```

Use version 3.2.0 (it should be tagged). Build the opencv using opencv contrib (look at README instuctions in both repos)

### Dlib
Clone dblib and build it:
```
git clone https://github.com/davisking/dlib.git
cd dlib; mkdir build ; cd build
cmake .. -DUSE_AVX_INSTRUCTIONS=ON ; cmake --build .
sudo make install
sudo ldconfig
```
last command when building on Linux

## Compiling the Frame Processor
use cmake:
```
cd frame_processor
mkdir build; cd build
cmake ../src ; cmake --build .
```
The executable is ./build/bin/frameprocessor
