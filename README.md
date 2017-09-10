# frame_processor
After cloning the repo run:
```
git submodule update --init --recursive
```
## Libs Installation
### OpenCV 3.2+
Clone opencv and opencv contrib:
```
git clone git@github.com:opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib
for each opencv & opencv_contrib
git tag -l 3.2.0
```

Use version 3.2.0 (it should be tagged). Build the opencv using opencv contrib (look at README instuctions in both repos)

### gRPC and protoc
It is going to be pain in the ass for many days, don't try to build this piece of crap on mac, it doesn't work as on august 2017, use brew instead:
```
brew install grpc
```
For Linux it should work as it advertised on their site (grpc.io), but I don't believe... Try it out. We still need this, until invent something better...

## Compiling the Frame Processor
Use cmake to build it. You must create (due to cpr dependency) build directory in the root of frame_processor:
```
cd frame_processor
mkdir build; cd build
cmake ..
make
```
The executable is ./build/bin/frameprocessor
