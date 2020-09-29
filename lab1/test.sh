#!/bin/bash
#modeling make check after sanity check 
chmod +x simpsh

TEMPDIR="temp_directory"
rm -rf $TEMPDIR
mkdir $TEMPDIR

cp simpsh ./$TEMPDIR/

cd $TEMPDIR

cat > x.txt <<EOF
test for make check \n
feebas \n
milotic \n
alomomola \n
luvdisc \n
EOF

cat x.txt > test1.txt
cat x.txt > test2.txt
cat x.txt > test3.txt
cat x.txt > test4.txt

echo "running make check:"
echo ""
echo
"Test case 1: 1 rdonly file and 1 wronly file"
./simpsh --rdonly test1.txt --creat --wronly test1one.txt
if [ $? -ne 0 ]
then
    echo "test case 1 failed"
else
    echo "test case 1 passed"
fi

echo ""
echo "Test case 2: --creat and --command with sort"
touch x_sorted.txt
touch y.txt
touch z.txt
sort x.txt > x_sorted.txt
./simpsh --rdonly test2.txt  --rdwr y.txt  --rdwr z.txt --command 0 1 2 sort
if [ $? -eq 0 ] && cmp y.txt x_sorted.txt 
then
    echo "test case 2 passed"
else
    echo "test case 2 failed"
fi


echo ""
echo "test case 3: --default, --catch and --abort"

./simpsh --catch 11 --default 11 --abort 
if [ $? == 139 ]
then
    echo "test case 3 passed"
else
    
    echo "test case 3 failed"
fi

echo ""
echo "Test case 4: --command without options"
./simpsh --rdonly x.txt --wronly z.txt --command 
if [ $? -ne 1 ]
then
    echo "test case 4 failed"
else
    echo "test case 4 passed" 
fi

cd ..
rm -rf $TEMPDIR
