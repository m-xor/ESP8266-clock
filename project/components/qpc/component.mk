#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS += app \
                             qep \
                             ../../main

COMPONENT_SRCDIRS += app \
                     qep
