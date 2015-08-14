#! /bin/sh
#                                               -*- Shell -*-
#
#  poutre.sh
#
#  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
#
#  This library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#  This file implements the 'poutre' model as an external code
#

progname="$0"

infiles="poutre_external_infile1 poutre_external_infile2"
outfile="poutre_external_outfile"

for file in $infiles
do
  if test -f "$file"
  then
    . $file
  else
    echo "$progname: ERROR ! Can't find input file '$file'"
    exit 1
  fi
done

cat <<EOF | bc > $outfile
scale=20
f=$F
l=$L
e=$E
i=$I
d=f*l*l*l/(3*e*i)
print "d=", d, "\n"
EOF

exit 0
