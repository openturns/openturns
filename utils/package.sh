#!/bin/sh

# this script creates source package

set -e

path=.
pkgname=openturns
obs=~/projects/science:openturns/$pkgname
pkgver=0.0
debver=$pkgver
rel=0.1

usage()
{
  echo "generate source package"
  echo ""
  echo "\t-h --help"
  echo "\t--path=$path"
  echo "\t--pkgname=$pkgname"
  echo "\t--pkgver=$pkgver"
  echo "\t--debver=$debver"
  echo "\t--rel=$rel"
  echo ""
}

while [ "$1" != "" ]; do
  PARAM=`echo $1 | awk -F= '{print $1}'`
  VALUE=`echo $1 | awk -F= '{print $2}'`
  case $PARAM in
    -h | --help)
      usage
      exit
      ;;
    --path)
      path=$VALUE
      ;;
    --pkgname)
      pkgname=$VALUE
      obs=~/projects/science:openturns/$pkgname
      ;;
    --pkgver)
      pkgver=$VALUE
      debver=$pkgver
      ;;
    --debver)
      debver=$VALUE
      ;;
    --rel)
      rel=$VALUE
      ;;
    --obs)
      obs=$VALUE
      ;;
      *)
    echo "ERROR: unknown parameter \"$PARAM\""
    usage
    exit 1
    ;;
  esac
  shift
done

echo "-- Creating $pkgname-$pkgver tarball from $path"

rm -rf /tmp/$pkgname-$pkgver
git archive HEAD --prefix=$pkgname-$pkgver/ | bzip2 > /tmp/$pkgname-$pkgver.tar.bz2
cd /tmp
tar xjf $pkgname-$pkgver.tar.bz2

rm -f "$pkgname"_$debver*
cp $pkgname-$pkgver.tar.bz2 "$pkgname"_$debver.orig.tar.bz2
cd /tmp/$pkgname-$pkgver

cp -r distro/debian .

# build source package
echo "-- Creating source package "$pkgname"_$debver-$rel"
debuild -us -uc -S --no-check-builddeps

# rpm files
echo "-- Copying files to $obs"
cp -v /tmp/$pkgname-$pkgver/distro/rpm/$pkgname.spec /tmp/$pkgname-$pkgver.tar.bz2 $obs
cp -v /tmp/"$pkgname"_$debver.orig.tar.bz2 /tmp/"$pkgname"_$debver-$rel.dsc /tmp/"$pkgname"_$debver-$rel.debian.tar.xz $obs


# build binary packages
#DEB_BUILD_OPTIONS="parallel=16" debuild -us -uc


