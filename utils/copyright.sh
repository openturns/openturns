#!/bin/sh
# update copyright date
newyear=`date +%Y`
oldyear=`expr $newyear - 1`
for subdir in lib python cmake
do
  grep -lr "2005-$oldyear Airbus-EDF-IMACS-ONERA-Phimeca" $subdir|grep -v '~'|xargs sed -i "s|2005-$oldyear Airbus-EDF-IMACS-ONERA-Phimeca|2005-$newyear Airbus-EDF-IMACS-ONERA-Phimeca|g"
done
