cp ./vfs/link.c /usr/src/minix/servers/vfs/link.c;
cp ./vfs/open.c /usr/src/minix/servers/vfs/open.c;
cp ./vfs/read.c /usr/src/minix/servers/vfs/read.c;
cp ./mfs/read.c /usr/src/minix/fs/mfs/read.c;
cp ./mfs/write.c /usr/src/minix/fs/mfs/write.c;
cp ./fslib.c /usr/src/minix/lib/libc/gen/fslib.c;
cp ./const.h /usr/src/minix/include/minix/const.h;
cd /usr/src;
make build MKUPDATE=yes >log.txt 2>log.txt;