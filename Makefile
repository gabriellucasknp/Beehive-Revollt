CC ?= gcc
BUILD_DIR ?= build
TARGET_NAME ?= beehive_revolt

ifeq ($(OS),Windows_NT)
TARGET_EXT ?= .exe
RAYLIB_PREFIX ?= C:/msys64/mingw64
endif

TARGET := $(BUILD_DIR)/$(TARGET_NAME)$(TARGET_EXT)
SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

CFLAGS ?= -Wall -Wextra -std=c99 -O2
CPPFLAGS += -MMD -MP
LDFLAGS ?=
LDLIBS ?=
PROJECT_LIBS ?= -lm

RAYLIB_CFLAGS ?= $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS ?= $(shell pkg-config --libs raylib 2>/dev/null)

ifeq ($(strip $(RAYLIB_CFLAGS)),)
ifneq ($(wildcard $(RAYLIB_PREFIX)/include/raylib.h),)
CPPFLAGS += -I$(RAYLIB_PREFIX)/include
endif
else
CPPFLAGS += $(RAYLIB_CFLAGS)
endif

ifeq ($(strip $(RAYLIB_LIBS)),)
ifneq ($(wildcard $(RAYLIB_PREFIX)/lib/libraylib.a),)
LDFLAGS += -L$(RAYLIB_PREFIX)/lib
endif
ifeq ($(OS),Windows_NT)
LDLIBS += -lraylib -lopengl32 -lgdi32 -lwinmm
else
LDLIBS += -lraylib
endif
else
LDLIBS += $(RAYLIB_LIBS)
endif

LDLIBS += $(PROJECT_LIBS)

.PHONY: all clean run

TEST_BIN := $(BUILD_DIR)/player_temp_boost_test
CAMPAIGN_TEST_BIN := $(BUILD_DIR)/campaign_test

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_BIN) $(CAMPAIGN_TEST_BIN)
	./$(TEST_BIN)
	./$(CAMPAIGN_TEST_BIN)

$(TEST_BIN): tests/player_temp_boost_test.c src/player.c src/player.h src/assets.h src/types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc tests/player_temp_boost_test.c src/player.c -o $@ $(LDFLAGS) $(LDLIBS)

$(CAMPAIGN_TEST_BIN): tests/campaign_test.c src/campaign.c src/campaign.h src/stage.h src/types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc tests/campaign_test.c src/campaign.c -o $@ $(LDFLAGS) $(LDLIBS)

-include $(DEPS)
