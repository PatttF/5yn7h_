
 

TARGET = 5yn7h_
FILES_DSP = src/DistrhoPlugin5yn7h_.cpp



all: print-dpf-path vst3 install


.PHONY: print-dpf-path
print-dpf-path:
	@echo "DPF_PATH is: $(DPF_PATH)"

# Output directory for builds
export DESTDIR := $(CURDIR)/bin


USER_VST3 := $(HOME)/.vst3


.PHONY: install
install:
	@if ls bin/*.vst3 1> /dev/null 2>&1; then \
	  mkdir -p $(USER_VST3); \
	  cp -vr bin/*.vst3 $(USER_VST3)/; \
	fi




# DPF include paths (must be set after all other logic)
BUILD_C_FLAGS   += -Isrc -I$(CURDIR)/src -I$(DPF_PATH)/distrho -I$(DPF_PATH)/dgl
BUILD_CXX_FLAGS += -Isrc -I$(CURDIR)/src -I$(DPF_PATH)/distrho -I$(DPF_PATH)/dgl

# DPF build include
include ../dpf/DPF/Makefile.plugins.mk

# Clean target
.PHONY: safe-clean
safe-clean:
	@echo "Cleaning only local build files..."
	rm -rf ./build ./bin *.o *.so *.dll *.dylib *.a *.vst3 *.clap
# Install directory for user VST3 plugins
