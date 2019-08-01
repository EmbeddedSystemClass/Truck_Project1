# to run this script: exec setARMpath.sh
#PATH=
#export PATH
PREFIX=
export PREFIX
#PREFIX1=/home/dan/dev/arm/crosstool/gcc-4.0.1-glibc-2.3.5/
#PREFIX1=/home/dan/dev/arm/crosstool/gcc3/
PREFIX1=/home/dan/dev/arm/crosstool/gcc4/
export PREFIX1
PREFIX2=arm-linux
export PREFIX2
PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$PREFIX1$PREFIX2/:$PREFIX1$PREFIX2/bin"
export PATH
TARGET=arm-elf
export TARGET
LIBDIR=$PREFIX1$PREFIX2/arm-linux/lib
export LIBDIR
CC=$PREFIX1/arm-linux/bin/arm-linux-gcc
export CC
CFLAGS="-mcpu=arm920t -mapcs-32 -Wstrict-prototypes"
export CFLAGS
LT_SYS_LIBRARY_PATH=$PREFIX1/arm-linux/arm-linux/lib
export LT_SYS_LIBRARY_PATH
LDFLAGS=-L$LT_SYS_LIBRARY_PATH
export LDFLAGS
CPPFLAGS=-I$PREFIX1/arm-linux/arm-linux/include
export CPPFLAGS
CPP=arm-linux-gcc
export CPP
CROSS_COMPILE=$PREFIX1$PREFIX2/
export CROSS_COMPILE
exec $SHELL -i
