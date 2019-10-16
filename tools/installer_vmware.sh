#!/bin/bash
# This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
# Copyright Airbus Group

if [ $(id -u) -ne 0 ]; then
    echo -e "\nError: must be root to update VMDK !\n"
fi

VMNAME="crashos"
VMPATH="tools/vmware/"
VMDISK="${VMPATH}/${VMNAME}.vmdk"
VMMNT="${VMPATH}/mnt"

vmware-mount "${VMDISK}" 1 "${VMMNT}" 2>/dev/null || exit 1
cp $1 "${VMMNT}/boot/$(basename $1)" || exit 1
vmware-mount -X 2>/dev/null
sleep .25 
vmware-mount -X 2>/dev/null
exit 0
