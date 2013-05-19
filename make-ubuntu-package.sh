#!/bin/bash
rm wildfox/usr/bin/WildFox.stub
cp WildFox wildfox/usr/bin/
fakeroot dpkg-deb --build wildfox wildfox_`grep Version wildfox/DEBIAN/control |sed -e 's/.* //'`_amd64.deb
