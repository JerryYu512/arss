# 目标
TARGET := brsdk
# 单元测试目标
UNIT_TEST_TARGET := $(TARGET)_ut
ROOT_DIR := $(shell pwd)

# 源码路径
SRC_DIR := brsdk
UT_DIR := unittest

# 临时目录
BUILD_DIR := build
DIST_DIR := dist

# 配置头文件
CONFIG_HEADER := $(BUILD_DIR)/configure.h

# 配置
include config.mk

# 源文件
SOURCES := $(shell find $(SRC_DIR) -type f -name *.cpp)
# 单元测试文件
UT_SOURCES := $(shell find $(UT_DIR) -type f -name *.cpp)

# 替换后缀
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/$(SRC_DIR)/%,$(SOURCES:.cpp=.o))
UT_OBJECTS := $(patsubst $(UT_DIR)/%, $(BUILD_DIR)/$(UT_DIR)/%,$(UT_SOURCES:.cpp=.o))

ifndef ECHO
HIT_TOTAL != ${MAKE} ${MAKECMDGOALS} --dry-run ECHO="HIT_MARK" | grep -c "HIT_MARK"
HIT_COUNT = $(eval HIT_N != expr ${HIT_N} + 1)${HIT_N}
ECHO = "[`expr ${HIT_COUNT} '*' 100 / ${HIT_TOTAL}`%]"
endif

# 临时依赖文件，用于分析每个.o文件依赖的头文件，在依赖的头文件变化时重新编译.o
DEPS := $(OBJECTS:%.o=%.d)
DEPS += $(UT_OBJECTS:%.o=%.d)

default: $(TARGET)

.PHONY: all
all: pack demo ut

# 生成配置头文件
.PHONY: config
config: $(ROOT_DIR)/$(SRC_DIR)/configure.h.in config.py
	@mkdir -p build
	@mkdir -p include
	@mkdir -p lib
	@echo "\033[32mpython3 config.py\033[0m"
	@python3 config.py --spath=$(ROOT_DIR)/$(SRC_DIR) --dpath=$(ROOT_DIR)/build \
	--version=$(RELEASE_VERSION) --build-version=$(BUILD_VERSION) --build-time="$(COMPILE_TIME)" \
	--compiler=$(CC) --debug=$(DEBUG)
	@echo "\033[35m[---- generic configure.h success ------]\033[0m"
	@echo ""

$(CONFIG_HEADER):config

$(TARGET): $(OBJECTS)
ifeq ($(ENABLE_SHREAD_LIB), y)
	@$(CXX) $(SHAREDFLG) $(OBJECTS) -o build/lib$@.so $(SO_LIBS) $(LIBS_PATH)
endif
ifeq ($(ENABLE_STATIC_LIB), y)
	@$(AR) build/lib$@.a $(OBJECTS)
endif
	@echo "\033[35m[---------- build lib success ----------]\033[0m"
	@echo ""

# 示例代码编译
.PHONY: demo
demo: $(TARGET)
	@make -C samples ARS_INC_DIR=$(ROOT_DIR)/include ARS_LIB_DIR=$(ROOT_DIR)/build OUTPUT_DIR=$(ROOT_DIR)/build/samples

# 单元测试
ut: $(TARGET) $(UT_OBJECTS)
	@$(CXX) $(LIBS_PATH) $(UT_OBJECTS) -l$(TARGET) -Lbuild $(ST_LIBS_UT) $(ST_LIBS) $(SO_LIBS) -o build/$(UNIT_TEST_TARGET)
	@echo "\033[35m[---------- build ut success -----------]\033[0m"
	@echo ""

# Doxygen文档
.PHONY: api
api:

.PHONY: install
install:
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk
	mkdir -p $(MAKE_INSTALL_DIR)/lib

ifeq ($(ENABLE_SHREAD_LIB), y)
	cp -rf build/lib$(TARGET).so $(MAKE_INSTALL_DIR)/lib/
endif
ifeq ($(ENABLE_STATIC_LIB), y)
	cp -rf build/lib$(TARGET).a $(MAKE_INSTALL_DIR)/lib/
endif

	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/atomic
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/co
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/crypto
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/defs
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/ds
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/encoding
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/err
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/event
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/fs
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/ini
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/flag
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/json
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/lock
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/log
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/mem
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/mix
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/net
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/os
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/plugin
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/process
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/pugixml
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/protocol
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/str
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/thread
	mkdir -p $(MAKE_INSTALL_DIR)/include/brsdk/time

	cp -rf brsdk/atomic/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/atomic/
	cp -rf brsdk/co/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/co/
	cp -rf brsdk/crypto/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/crypto/
	cp -rf brsdk/defs/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/defs/
	cp -rf brsdk/ds/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/ds/
	cp -rf brsdk/encoding/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/encoding/
	cp -rf brsdk/err/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/err/
	# cp -rf brsdk/event/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/event/
	cp -rf brsdk/fs/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/fs/
	cp -rf brsdk/ini/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/ini/
	cp -rf brsdk/flag/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/flag/
	cp -rf brsdk/json/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/json/
	cp -rf brsdk/lock/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/lock/
	cp -rf brsdk/log/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/log/
	cp -rf brsdk/mem/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/mem/
	cp -rf brsdk/mix/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/mix/
	cp -rf brsdk/net/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/net/
	cp -rf brsdk/os/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/os/
	cp -rf brsdk/plugin/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/plugin/
	cp -rf brsdk/process/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/process/
	cp -rf brsdk/pugixml/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/pugixml/
	cp -rf brsdk/protocol/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/protocol/
	cp -rf brsdk/str/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/str/
	cp -rf brsdk/thread/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/thread/
	cp -rf brsdk/time/*.h* $(MAKE_INSTALL_DIR)/include/brsdk/time/
	cp -rf brsdk/brsdk.hpp $(MAKE_INSTALL_DIR)/include/brsdk/
	cp -rf build/configure.h $(MAKE_INSTALL_DIR)/include/brsdk/

# 打包
.PHONY: pack
pack: $(TARGET) api
# 	@echo "\033[35m[----------- package start -------------]\033[0m"
# 	@rm -rf $(DIST_DIR)
# 	@mkdir -p $(DIST_DIR)/include/ars
# 	@mkdir -p $(DIST_DIR)/lib
# 	@mkdir -p $(DIST_DIR)/docs
# 	@mkdir -p $(DIST_DIR)/demo

# 	@echo "\033[32mcopy samples\033[0m"
# 	@cp -f samples/*.cpp $(DIST_DIR)/demo/
# 	@cp -f samples/Makefile $(DIST_DIR)/demo/

# 	@echo "\033[32mcopy docs\033[0m"
# 	@cp -rf docs/usr/* $(DIST_DIR)/docs/
# 	@cp -f README.md $(DIST_DIR)/

# 	@echo "\033[32mcopy libs\033[0m"
# 	@cp -f build/lib$(TARGET).so $(DIST_DIR)/lib/

# 	@echo "\033[32mcopy headers\033[0m"
# 	@cp include/ars/sdk $(DIST_DIR)/include/ars/ -rf

# 	@echo "\033[32mtar -zcf ars-$(PLAT_NAME)-$(RELEASE_VERSION)-$(MODE).tar.gz include lib demo docs README.md\033[0m"
# 	@cd $(DIST_DIR) && tar -zcf ars-$(PLAT_NAME)-$(RELEASE_VERSION)-$(MODE).tar.gz include lib demo docs README.md
# 	@mkdir -p target/$(PLAT_NAME)/$(MODE)/

# 	@echo "\033[32mcopy ars-$(PLAT_NAME)-$(RELEASE_VERSION)-$(MODE).tar.gz to target/$(PLAT_NAME)/$(MODE)/\033[0m"
# 	@cp $(DIST_DIR)/ars-$(PLAT_NAME)-$(RELEASE_VERSION)-$(MODE).tar.gz target/$(PLAT_NAME)/$(MODE)/

# 	@echo "\033[35m[---------- package success ------------]\033[0m"
# 	@echo ""

.PHONY: clean
clean:
	@echo clean
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DIST_DIR)

# dependencies
-include $(DEPS)
$(BUILD_DIR)/%.o: %.c* $(CONFIG_HEADER)
	@echo "$(ECHO) \033[32m$(CXX) $<\033[0m"
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi;\
	$(CXX) $(OBJCCFLAG) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<; \
	$(CXX) $(OBJCCFLAG) $< -o $@
