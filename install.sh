#!/bin/bash
curl -L https://raw.github.com/emin/WebPQuickLook/master/WebpQuickLook.tar.gz | tar -xvz
mkdir -p ~/Library/QuickLook/
mv WebpQuickLook.qlgenerator ~/Library/QuickLook/
qlmanage -r

