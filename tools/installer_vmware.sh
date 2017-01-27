#!/bin/bash

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
