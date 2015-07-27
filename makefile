# Simply Embedded tutorial makefile

# Path to the msp430 toolchain
TOOLCHAIN_ROOT?=/opt/msp430-toolchain

# Toolchain variables
CC:=$(TOOLCHAIN_ROOT)/bin/msp430-gcc

# Directories
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
BIN_DIR=$(BUILD_DIR)/bin
SRC_DIR=src
INC_DIR=include

# Attempt to create the output directories
ifneq ($(BUILD_DIR),)
$(shell [ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR))
$(shell [ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR))
$(shell [ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR))
endif 

# Source files
SRCS:=$(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS:=$(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

# ELF file output
ELF:=$(BIN_DIR)/app.out

# Compile flags
CFLAGS:= -mmcu=msp430g2553 -mhwmult=none -c -O0 -g3 -ggdb -gdwarf-2 -Wall -Werror -Wextra -Wshadow -std=gnu90 -Wpedantic -MMD -I$(INC_DIR)

# Linker flags
LDFLAGS:= -mmcu=msp430g2553

# Dependancies
DEPS:=$(OBJS:.o=.d)

# Rules
.PHONY: all
all: $(ELF)

$(ELF) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean: 
	rm -rf $(BUILD_DIR)

-include $(DEPS)
