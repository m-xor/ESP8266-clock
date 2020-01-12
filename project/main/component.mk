#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#to generate date of compilation. Is updated at every 'make' command
CHEAT_ARG := $(shell rm -f buildstamp.o)

# updating fsdata.h file which constitutes ROM filesystem for web api files
fs.o: fsdata.h
fsdata.h: $(PROJECT_PATH)/../html/*.*
	cd $(PROJECT_PATH)/.. ; \
          $(QPT_PATH)/qfsgen html $(COMPONENT_BUILD_DIR)/fsdata.h; \
          cd $(COMPONENT_BUILD_DIR)