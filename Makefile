# 目标
TARGET := arss
# 单元测试目标
UNIT_TEST_TARGET := $(TARGET)_ut
ROOT_DIR := $(shell pwd)

# 源码路径
SRC_DIR := arss
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

# 临时依赖文件，用于分析每个.o文件依赖的头文件，在依赖的头文件变化时重新编译.o
DEPS := $(OBJECTS:%.o=%.d)
DEPS += $(UT_OBJECTS:%.o=%.d)

default: $(TARGET)

.PHONY: all
all: pack demo ut

# 生成配置头文件
.PHONY: config
config: $(ROOT_DIR)/$(SRC_DIR)/configure.h.in
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
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss
	mkdir -p $(MAKE_INSTALL_DIR)/lib

	cp -rf build/lib$(TARGET).so $(MAKE_INSTALL_DIR)/lib/
	cp -rf build/lib$(TARGET).a $(MAKE_INSTALL_DIR)/lib/

	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/atomic
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/co
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/crypto
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/defs
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/ds
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/encoding
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/err
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/event
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/fs
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/ini
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/json
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/lock
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/log
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/mem
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/mix
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/net
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/os
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/plugin
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/process
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/pugixml
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/protocol
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/str
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/thread
	mkdir -p $(MAKE_INSTALL_DIR)/include/arss/time

	cp -rf arss/atomic/*.h* $(MAKE_INSTALL_DIR)/include/arss/atomic/
	cp -rf arss/co/*.h* $(MAKE_INSTALL_DIR)/include/arss/co/
	cp -rf arss/crypto/*.h* $(MAKE_INSTALL_DIR)/include/arss/crypto/
	cp -rf arss/defs/*.h* $(MAKE_INSTALL_DIR)/include/arss/defs/
	cp -rf arss/ds/*.h* $(MAKE_INSTALL_DIR)/include/arss/ds/
	cp -rf arss/encoding/*.h* $(MAKE_INSTALL_DIR)/include/arss/encoding/
	cp -rf arss/err/*.h* $(MAKE_INSTALL_DIR)/include/arss/err/
	cp -rf arss/event/*.h* $(MAKE_INSTALL_DIR)/include/arss/event/
	cp -rf arss/fs/*.h* $(MAKE_INSTALL_DIR)/include/arss/fs/
	cp -rf arss/ini/*.h* $(MAKE_INSTALL_DIR)/include/arss/ini/
	cp -rf arss/json/*.h* $(MAKE_INSTALL_DIR)/include/arss/json/
	cp -rf arss/lock/*.h* $(MAKE_INSTALL_DIR)/include/arss/lock/
	cp -rf arss/log/*.h* $(MAKE_INSTALL_DIR)/include/arss/log/
	cp -rf arss/mem/*.h* $(MAKE_INSTALL_DIR)/include/arss/mem/
	cp -rf arss/mix/*.h* $(MAKE_INSTALL_DIR)/include/arss/mix/
	cp -rf arss/net/*.h* $(MAKE_INSTALL_DIR)/include/arss/net/
	cp -rf arss/os/*.h* $(MAKE_INSTALL_DIR)/include/arss/os/
	cp -rf arss/plugin/*.h* $(MAKE_INSTALL_DIR)/include/arss/plugin/
	cp -rf arss/process/*.h* $(MAKE_INSTALL_DIR)/include/arss/process/
	cp -rf arss/pugixml/*.h* $(MAKE_INSTALL_DIR)/include/arss/pugixml/
	cp -rf arss/protocol/*.h* $(MAKE_INSTALL_DIR)/include/arss/protocol/
	cp -rf arss/str/*.h* $(MAKE_INSTALL_DIR)/include/arss/str/
	cp -rf arss/thread/*.h* $(MAKE_INSTALL_DIR)/include/arss/thread/
	cp -rf arss/time/*.* $(MAKE_INSTALL_DIR)/include/arss/time/
	cp -rf arss/arss.hpp $(MAKE_INSTALL_DIR)/include/arss/
	cp -rf build/configure.h $(MAKE_INSTALL_DIR)/include/arss/

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
	@echo "\033[32m$(CXX) $<\033[0m"
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi;\
	$(CXX) $(OBJCCFLAG) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<; \
	$(CXX) $(OBJCCFLAG) $< -o $@
