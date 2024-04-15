#!/bin/bash
set -e
set -x
rm -rf $HOME/.aptly pkgs
pkgs="qtmultimedia5-dev, libdtkwidget-dev, libqt5multimediawidgets5, libqt5multimedia5, libqt5svg5-dev, qttools5-dev, libqt5x11extras5-dev, libmpv-dev, libxcb1-dev, libxcb-util0-dev, libffmpegthumbnailer-dev, libxcb-shape0-dev, libxcb-ewmh-dev, xcb-proto, x11proto-record-dev, libxtst-dev, libavcodec-dev, libavformat-dev, libavutil-dev, libpulse-dev, libdvdnav-dev, libgsettings-qt-dev, libmpris-qt5-dev, libdbusextended-qt5-dev, libva-dev, libgstreamer-plugins-base1.0-dev, libgstreamer1.0-dev"
pkgs=$(echo "$pkgs" | tr ',' '|')
url=http://pools.uniontech.com/desktop-professional
distribution=eagle
components="main contrib"
arch=amd64
aptly mirror create -ignore-signatures -architectures=$arch -filter="$pkgs" -filter-with-deps linglong-download-depend $url $distribution $components
aptly mirror update -ignore-signatures linglong-download-depend
find ~/.aptly/pool -type f
mv ~/.aptly/pool pkgs