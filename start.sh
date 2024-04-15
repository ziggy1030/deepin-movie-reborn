#!/usr/bin/env bash
export LD_LIBRARY_PATH=/opt/apps/org.deepin.movie/files/lib/x86_64-linux-gnu:/opt/apps/org.deepin.movie/files/lib/x86_64-linux-gnu:
cd /opt/apps/org.deepin.movie/files/bin && ./deepin-movie $@
