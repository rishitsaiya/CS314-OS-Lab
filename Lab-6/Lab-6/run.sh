cp open.c /usr/src/minix/servers/vfs/open.c
cp link.c /usr/src/minix/servers/vfs/link.c
cp read.c /usr/src/minix/servers/vfs/read.c
cd /usr/src
make build MKUPDATE=yes
