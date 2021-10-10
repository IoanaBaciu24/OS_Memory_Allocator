#/bin/bash

make -B mem_shell
ls tests > tests.list
files=`sed -i -e 's/.in/.test/g' tests.list`
files=`cat tests.list`
for tests in $files
do
    make -B tests/$tests | grep -E "Passed|Failed"
done

rm tests.list

