#
# SPDX-License-Identifier: LGPL-2.1-only
# -------------------------------*-TayhuangOS-*-----------------------------------
# 
#              Copyright (C) 2022, 2022 TayhuangOS Development Team
# 
# --------------------------------------------------------------------------------
# 
# 作者: theflysong 383494
# 
# Makefile
# 
# Makefile
# 
#



# 脚本所在目录

ROOTDIR := $(shell pwd)
SCRIPTDIR := $(ROOTDIR)/script/

export ROOTDIR SCRIPTDIR

include $(SCRIPTDIR)/config.mk
include $(SCRIPTDIR)/version.mk
include $(SCRIPTDIR)/directory.mk
include $(SCRIPTDIR)/tools/tool.mk

# 本来应该放在定义区，但依赖于get_loop.sh
LOOPA ?= $(shell $(GET_LOOP) 0)
LOOPB ?= $(shell $(GET_LOOP) $(LOOPA))

BUILD_LIBS := false
BUILD_LOADER := true
BUILD_KERNEL := false
BUILD_MODULE := false

include ./misc.mk
include ./setup.mk
include ./image.mk
include ./run.mk

# 任务区
# 下划线结尾的任务不应直接调用

#编译并写入映像
.PHONY: all
all: build image
	$(ECHO) "done"

#重编译并写入映像
.PHONY: rebuild
rebuild: clean build image
	$(ECHO) "done"

#编译
.PHONY: build
build:
	$(ECHO) "TayhuangOS Version: $(VERSION)"
ifeq ($(BUILD_LIBS), true)
	$(CD) libs && $(MAKE) build
endif
ifeq ($(BUILD_LOADER), true)
	$(CD) loader && $(MAKE) build
endif
ifeq ($(BUILD_KERNEL), true)
	$(CD) kernel && $(MAKE) build
endif
ifeq ($(BUILD_MODULE), true)
	$(CD) module && $(MAKE) build
endif
ifeq ($(VBE_MODE), ENABLE)
	$(MAKE) $(RAW_ICON)
endif

#清理
.PHONY: clean
clean:
	$(ECHO) "TayhuangOS Version: $(VERSION)"
ifeq ($(BUILD_LIBS), true)
	-$(CD) libs && $(MAKE) clean
endif
ifeq ($(BUILD_LOADER), true)
	-$(CD) loader && $(MAKE) clean
endif
ifeq ($(BUILD_KERNEL), true)
	-$(CD) kernel && $(MAKE) clean
endif
ifeq ($(BUILD_MODULE), true)
	-$(CD) module && $(MAKE) clean
endif