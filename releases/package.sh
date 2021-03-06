#!/bin/bash
pkgname=frproxy-1.1.7

if [ -d ~/rpmbuild/ ]; then
    rpmb_root=~/rpmbuild
else
    rpmb_root=/usr/src/redhat
fi
cd "$(dirname $0)"
tmp=$(dirname $0)/tmp
folder=$tmp/$pkgname
test -d $tmp/ || mkdir -- $tmp
test -d $folder/ || mkdir -- $folder/

echo $pkgname
echo $folder
echo packaging $pkgname
echo package dest $rpmb_root

cp -f ../bin/frproxy $folder/
cp -f ../bin/client $folder/
cp -f ../proxy.sh $folder/
cp -f ../proxy_client.sh $folder/
cp -f ../lib/* $folder/
cp -f ../releases/frproxy.init.d.sh $folder/frproxy.init.d.sh

cd $tmp/
tar zcvf "$pkgname.tar.gz" "$pkgname/"
cd ..
echo ..............................

if [ -f $tmp/$pkgname.tar.gz ] ; then
    ls $tmp/
    mv $tmp/$pkgname.tar.gz $rpmb_root/SOURCES/$pkgname.tar.gz
    cp package.spec "$rpmb_root/SPECS/"
    set -xv
    #QA_RPATHS=$[ 0x0004|0x0002|0x00FF ] rpmbuild -bb --define "_binary_filedigest_algorithm  1"  --define "_binary_payload 1" package.spec
    QA_RPATHS=$[ 0x0004|0x0002 ] rpmbuild -bb --define "_binary_filedigest_algorithm  1"  --define "_binary_payload 1" package.spec
    STATUS=$?
else
    echo failure. package source $pkgname.tar.gz not exists. 
fi

test -f $rpmb_root/SOURCES/$pkgname.tar.gz && rm -fr $rpmb_root/SOURCES/$pkgname.tar.gz
test -f "$rpmb_root/SPECS/package.spec" && rm -fr "$rpmb_root/SPECS/package.spec"

rm -fr "$tmp/$pkgname/"

if [ $STATUS -eq 0 ]; then
    cp -f $rpmb_root/RPMS/x86_64/$pkgname*.rpm $tmp/
    test -f $rpmb_root/RPMS/x86_64/$pkgname*.rpm && rm -fr $rpmb_root/RPMS/x86_64/$pkgname*.rpm
    echo package successfully.
    ls -ahl tmp/
else

    echo fail to build $pkname, check it out please.
fi
