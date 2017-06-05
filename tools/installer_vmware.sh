#!/bin/bash
# This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
# Copyright Airbus Group

VMNAME="crashos"
VMPATH="/home/anais/Vmware/${VMNAME}"
VMDISK="${VMPATH}/${VMNAME}.vmdk"
VMMNT="${VMPATH}/mnt"

vmware-mount "${VMDISK}" 1 "${VMMNT}" 2>/dev/null || exit 1
cp $1 "${VMMNT}/$(basename $1)" || exit 1
vmware-mount -X 2>/dev/null
sleep .25 
vmware-mount -X 2>/dev/null
exit 0
