#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS += include \
                             src \
                             app \
                             freertos

COMPONENT_SRCDIRS += app \
                     freertos \
                     include \
                     src/qf 

ifdef CONFIG_USING_QS
COMPONENT_SRCDIRS += src/qs
endif