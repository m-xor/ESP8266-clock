#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#
# Component Makefile
#

# In components/qpc create symlink 'qpc' to the main QPC directiory
# Copy files in $(QPC_PATH)/ports/freertos directory to components/qpc/port 
#   (QF_MAX_ACTIVE needs adjusting to be in sync with FreeRTOS' configMAX_PRIORITIES)
COMPONENT_ADD_INCLUDEDIRS += app \
                             qpc/include \
                             qpc/src \
                             port

COMPONENT_SRCDIRS += app \
                     qpc/include \
                     qpc/src/qf \
                     port

ifdef CONFIG_USING_QS
COMPONENT_SRCDIRS += qpc/src/qs
CPPFLAGS += -DQ_SPY
endif