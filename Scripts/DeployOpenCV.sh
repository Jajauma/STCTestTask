#!/bin/bash

set -e

targetVersion=3.4.2
targetURL=https://github.com/opencv/opencv/archive/"$targetVersion".tar.gz
workDir=DeployOpenCV
buildDir=$workDir/Build
installPrefix=/var/tmp/OpenCV

rm -rf "$buildDir" "$installPrefix"
mkdir -p "$buildDir"

wget --no-clobber \
    --content-disposition \
    --directory-prefix="$workDir" \
    "$targetURL"
tar -xf "$workDir/opencv-$targetVersion.tar.gz" -C "$buildDir"

(
mkdir -p "$buildDir/ImgCodecsOnly"
cd "$buildDir/ImgCodecsOnly"
cmake \
    -DCMAKE_INSTALL_PREFIX="$installPrefix/$targetVersion-ImgCodecsOnly" \
    -DCMAKE_BUILD_TYPE=Release \
    \
    -DBUILD_opencv_core=ON \
    -DBUILD_opencv_imgcodecs=ON \
    -DBUILD_opencv_imgproc=ON \
    \
    -DWITH_IMGCODEC_HDR=ON \
    -DWITH_IMGCODEC_PXM=ON \
    -DWITH_IMGCODEC_SUNRASTER=ON \
    -DWITH_JASPER=ON \
    -DWITH_JPEG=ON \
    -DWITH_OPENEXR=ON \
    -DWITH_PNG=ON \
    -DWITH_TIFF=ON \
    -DWITH_WEBP=ON \
    \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_calib3d=OFF \
    -DBUILD_opencv_dnn=OFF \
    -DBUILD_opencv_features2d=OFF \
    -DBUILD_opencv_flann=OFF \
    -DBUILD_opencv_highgui=OFF \
    -DBUILD_opencv_java_bindings_generator=OFF \
    -DBUILD_opencv_ml=OFF \
    -DBUILD_opencv_objdetect=OFF \
    -DBUILD_opencv_photo=OFF \
    -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python_bindings_generator=OFF \
    -DBUILD_opencv_shape=OFF \
    -DBUILD_opencv_stitching=OFF \
    -DBUILD_opencv_superres=OFF \
    -DBUILD_opencv_ts=OFF \
    -DBUILD_opencv_video=OFF \
    -DBUILD_opencv_videoio=OFF \
    -DBUILD_opencv_videostab=OFF \
    \
    -DBUILD_JAVA=OFF \
    -DWITH_EIGEN=OFF \
    -DWITH_FFMPEG=OFF \
    -DWITH_GPHOTO2=OFF \
    -DWITH_GSTREAMER=OFF \
    -DWITH_GTK=OFF \
    -DWITH_IPP=OFF \
    -DWITH_ITT=OFF \
    -DWITH_LAPACK=OFF \
    -DWITH_MATLAB=OFF \
    -DWITH_OPENCL=OFF \
    -DWITH_PROTOBUF=OFF \
    -DWITH_V4L=OFF \
    -DWITH_VTK=OFF \
    \
    ../opencv-"$targetVersion"
make -j"$(nproc)"
make install
)
