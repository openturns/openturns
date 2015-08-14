#!/bin/sh

PATH=$PATH:/bin

OT_PREFIX="C:\Python27\Lib\site-packages\openturns"
cd $OT_PREFIX\\examples
# add python dir
PATH=$PATH:$PWD/../../../..

FAILED_TEST=""
function check_error {
  if [ "$?" != "0" ]; then
    FAILED_TEST="$FAILED_TEST $1"
  fi
}


echo "./start_check_test.sh"
./start_check_test.sh
check_error "./start_check_test.sh"


echo "./start_installcheck_test.sh"
./start_installcheck_test.sh
check_error "./start_installcheck_test.sh"


echo "./start_pythoninstallcheck_test.sh"
./start_pythoninstallcheck_test.sh
check_error "./start_pythoninstallcheck_test.sh"


RC=0
if [ "$FAILED_TEST" = "" ]; then
  echo "Every check tests are OK."
else
  echo "Some tests in$FAILED_TEST failed!"
  RC=1
fi

echo
echo "Press any key to exit."
read ANYKEY

exit $RC


