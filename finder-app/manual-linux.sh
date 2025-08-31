#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

# modified for ECEA 5305 assignment 3, part 2 by Larry

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

make="$(type -p make) ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE"
sysroot=$(${CROSS_COMPILE}gcc --print-sysroot)
sourcedir="$PWD"

if [ $# -lt 1 ]
then
    echo "Using default directory ${OUTDIR} for output"
else
    OUTDIR="$1"
    echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p "${OUTDIR}"

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
    echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
    git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e "${OUTDIR}"/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # DONE: Add your kernel build steps here
    $make mrproper
    $make defconfig
    $make -j8 all
    $make -j8 modules
    $make dtbs
fi

echo "Adding the Image in outdir"
cp -p "$OUTDIR"/linux-stable/arch/arm64/boot/Image "$OUTDIR"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
    echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf "${OUTDIR}"/rootfs
fi

# DONE: Create necessary base directories
mkdir -p "$OUTDIR"/rootfs
cd       "$OUTDIR"/rootfs
mkdir -p bin dev etc lib lib64 proc sys sbin tmp usr/bin usr/sbin var/log home

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
    git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # DONE:  Configure busybox
    $make defconfig
else
    cd busybox
fi

# DONE: Make and install busybox
$make -j8 CONFIG_PREFIX="$OUTDIR"/rootfs install

cd "$OUTDIR"/rootfs

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# DONE: Add library dependencies to rootfs
cp -p "$sysroot"/lib/ld-linux-aarch64.so.1 lib
cp -p "$sysroot"/lib64/{libm.so.6,libresolv.so.2,libc.so.6} lib64

# DONE: Make device nodes
sudo mknod -m 666 dev/null    c 1 3
sudo mknod -m 666 dev/console c 5 1

# DONE: Clean and build the writer utility
cd "$sourcedir"
make CROSS_COMPILE=$CROSS_COMPILE clean
make CROSS_COMPILE=$CROSS_COMPILE

# DONE: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp -a writer finder* autorun-qemu.sh conf "$OUTDIR"/rootfs/home/
cp -a ../conf                             "$OUTDIR"/rootfs/

# DONE: Chown the root directory
sudo chown -R root:root "$OUTDIR"/rootfs

# DONE: Create initramfs.cpio.gz
cd "$OUTDIR"/rootfs
find | cpio -H newc -ov --owner root:root | gzip > "$OUTDIR"/initramfs.cpio.gz
