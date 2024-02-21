TARGET = fft
OPT_LVL = 3
LIBRARIES = GL GLU glfw


BUILD_DIR = build
INC_DIR = include
SRC_DIR = src

SRC = $(wildcard $(SRC_DIR)/*.c)
INC = $(wildcard $(INC_DIR)/*.h)
OBJ = $(subst $(SRC_DIR),$(BUILD_DIR),$(subst .c,.o,$(SRC)))
LIB = $(addprefix -l, $(LIBRARIES))

CC = clang
LINKER = clang

CC_FLAGS = -c -g -Wall -Wextra -Wpedantic -Werror -I$(INC_DIR)/ -O$(OPT_LVL)
LINKER_FLAGS = -g -O$(OPT_LVL) -o $(TARGET) $(OBJ) $(LIB)

.PHONY: all clean

all: $(TARGET) makefile

$(TARGET): $(OBJ) $(INC) $(SHH) makefile
	$(LINKER) $(LINKER_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC) $(SHH) makefile | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f $(TARGET)
