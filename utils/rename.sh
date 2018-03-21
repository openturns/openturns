#!/bin/sh

# rename a class

usage()
{
  echo "Usage: $0 OldClass NewClass"
  exit 1
}

test $# = 2 || usage


files=`find lib python -name ${1}[\._]*`
t_files=`find lib python -name t_${1}_*`
for src_file in ${files} ${t_files}
do
  parent_dir=`dirname ${src_file}`
  dest_file=`basename ${src_file} | sed "s|$1|$2|g"`
  git mv ${src_file} ${parent_dir}/${dest_file}
done

upper_src=`echo $1 | tr "[:lower:]" "[:upper:]"`
upper_dest=`echo $2 | tr "[:lower:]" "[:upper:]"`
grep -lr $1 lib python|grep -v '~'|xargs sed -i "s|\b$1\b|$2|g;s|\b$1_|$2_|g;s|_$1\b|_$2|g"
find lib python -name $2.hxx | xargs sed -i "s|_${upper_src}_HXX|_${upper_dest}_HXX|g"
