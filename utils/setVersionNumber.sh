#!/bin/bash

usage()
{
  echo "Usage: $0 new_version"
  exit 1
}

error()
{
  echo "File VERSION is missing. Abort."
  exit 1
}

[[ $# == 1 ]] || usage
version=$1

[[ -f VERSION ]] || error
oldversion=`cat VERSION`

echo "Changing version from $oldversion to $version"

oldmajor=`echo $oldversion | cut -d "." -f 1`
oldminor=`echo $oldversion | cut -d "." -f 2`
oldpatch=`echo $oldversion | cut -d "." -f 3`
major=`echo $version | cut -d "." -f 1`
minor=`echo $version | cut -d "." -f 2`
patch=`echo $version | cut -d "." -f 3`

sed -i.bak "s/CPACK_PACKAGE_VERSION_MAJOR $oldmajor/CPACK_PACKAGE_VERSION_MAJOR $major/g" CMakeLists.txt
sed -i.bak "s/CPACK_PACKAGE_VERSION_MINOR $oldminor/CPACK_PACKAGE_VERSION_MINOR $minor/g" CMakeLists.txt

number_of_dots=`grep -o '\.' <<< $version | wc -l`
if test $number_of_dots -ge 2
then
  sed -i.bak "s/set (CPACK_PACKAGE_VERSION_PATCH $oldpatch/set (CPACK_PACKAGE_VERSION_PATCH $patch/g" CMakeLists.txt
fi
rm CMakeLists.txt.bak

echo $version > VERSION
echo "Updated file VERSION with new version $version"

exit 0
