#!/bin/sh

#set -e

export PRINTF_EXPONENT_DIGITS=2
export AUTOTEST_PATH=$PWD
export OPENTURNS_CONFIG_PATH=$PWD/..
#export OPENTURNS_LOG_SEVERITY=ALL
DEBUG_FILE=stdouterr.dbg 
PATH=$PWD:$PWD/..:$PATH
FAILED_TEST=""

function check_error {
if [ "$?" != "0" ]; then
  FAILED_TEST="$FAILED_TEST $1"
  echo "std_err output:"
  cat $DEBUG_FILE
  echo "Test #$1: $2 failed!"
  echo
fi
}


start_tests() {
test_files=`ls t_*.exe`
NB_CHECK=`ls t_*.exe|wc -l|tr -d " "`
CUR_CHECK=1
for test_file in $test_files
do
  test_base=`basename $test_file .exe`
  echo "Test #${CUR_CHECK}/$NB_CHECK: $test_base"
  if test -f $test_base.expout
  then
    (./$test_base 2>$DEBUG_FILE | tr -d '\r' > $test_base.out) && diff -u $test_base.expout $test_base.out
  else
    ./$test_base > $DEBUG_FILE 2>&1
  fi
  check_error ${CUR_CHECK} $test_base
  CUR_CHECK=`expr ${CUR_CHECK} + 1`
done
}

start_tests
echo
RC=0
if [ "$FAILED_TEST" = "" ]; then
  echo "Every check tests are OK."
else
  echo "Tests$FAILED_TEST failed!"
  RC=1
fi
exit $RC
