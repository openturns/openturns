#!/bin/sh

# rename a class

usage()
{
  echo "Usage: $0 OldClass NewClass"
  exit 1
}

test $# = 2 || usage


files=`find lib python -name ${1}*`
t_files=`find lib python -name t_${1}*`
for src_file in ${files} ${t_files}
do
  dest_file=`echo ${src_file} | sed "s|$1|$2|g"`
  git mv ${src_file} ${dest_file}
done

upper_src=`echo $1 | tr "[:lower:]" "[:upper:]"`
upper_dest=`echo $2 | tr "[:lower:]" "[:upper:]"`
find lib python -name $2.hxx | xargs sed -i "s|$upper_src|$upper_dest|g"
grep -lr $1 lib python|grep -v '~'|xargs sed -i "s|$1|$2|g"
