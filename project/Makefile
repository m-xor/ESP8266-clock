#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ntp_clock

EXCLUDE_COMPONENTS=coap jsmn libsodium mqtt

# requires by QPC
CPPFLAGS += -DconfigSUPPORT_STATIC_ALLOCATION=1
# QSpy tool
#ifdef CONFIG_USING_QS
#CFLAGS += -DQ_SPY
#CPPFLAGS += -DQ_SPY
#endif

include $(IDF_PATH)/make/project.mk

