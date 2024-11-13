#/*
# * Copyright 2023 Ethan. All rights reserved.
# */

.PHONY: $(EXEC) $(LIB)

#
# Rule to build executable
#
ifdef EXEC
$(EXEC): $(BUILD)/$(EXEC)

$(BUILD)/$(EXEC): $(LIBDEPS) $(OBJS) | $(BUILD)
	    $(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS) -Wl,-Map=$@.map -Wl,-rpath-link=$(TOOLCHAIN_DIR)/lib
endif

#
# Rule to make static and shared libraries
#
ifdef LIB
LIB_A := $(filter %.a,$(LIB))
LIB_SO := $(filter %.so \
	%.so.$(MAJOR_VERSION) \
	%.so.$(MAJOR_VERSION).$(MINOR_VERSION) \
	%.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(MAINTENANCE_VERSION),$(LIB))
LIB_BUILD :=

ifneq ($(LIB_A),)
LIB_BUILD += $(BUILD)/$(LIB_A)
$(BUILD)/$(LIB_A): $(OBJS) | $(BUILD)
	$(AR) r $@ $(OBJS)
endif

ifneq ($(LIB_SO),)
LIB_BUILD += $(BUILD)/$(LIB_SO)
$(BUILD)/$(LIB_SO): $(OBJS) | $(BUILD)
	$(CXX) -shared -fPIC $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
endif

ifeq ($(LIB_BUILD),)
$(error Unsupported LIB type: $(LIB) ***)
endif

$(LIB): $(LIB_BUILD)
endif

#
# Object file rules
#
$(BUILD)/%.o: %.c Makefile
	mkdir -p $(dir $@) ; $(CC) -c $(CFLAGS) -o $@ $<

#
# Object file rules
#
$(BUILD)/%.o: %.cpp Makefile
	mkdir -p $(dir $@) ; $(CXX) -c $(CXXFLAGS) -o $@ $<

#
# Generate build directory
#
$(BUILD):
	mkdir -p $@

.PHONY: clean

clean:
	rm -f $(OBJS)
	cd $(BUILD); rm -f $(EXEC) $(LIB)
