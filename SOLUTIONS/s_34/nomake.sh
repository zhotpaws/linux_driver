#!/bin/bash
#/* **************** LFD331:4.2 s_34/nomake.sh **************** */
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
#!/bin/bash

PATH=../:$PATH
export LDLIBS="-lfuse"
KROOT=$KROOT ../genmake 
make
