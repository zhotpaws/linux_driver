#!/bin/bash
#/* **************** LFD331:4.2 s_05/lab4_unloadit.sh **************** */
#/*
# * The code herein is: Copyright the Linux Foundation, 2015
# *
# * This Copyright is retained for the purpose of protecting free
# * redistribution of source.
# *
# *     URL:    http://training.linuxfoundation.org
# *     email:  trainingquestions@linuxfoundation.org
# *
# * This code is distributed under Version 2 of the GNU General Public
# * License, which you should have received with the source.
# *
# */
#!/bin/sh

# 1/2005 J. Cooperstein (coop@linuxfoundation.org) License:GPLv2

module=lab4_dynamic.ko
node=/dev/mycdrv
minor=0

[[ -n $1 ]] && module="$1"
[[ -n $2 ]] && node="$2"


major=$(awk "\$2==\"mycdrv\" {print \$1}" /proc/devices)
echo major number is: $major

echo unloading $module
rmmod $module

echo removing the device node at $node with minor number=0

rm $node
