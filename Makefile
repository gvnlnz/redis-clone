CC := gcc
CFLAGS := -Werror -Wextra -Wall
TARGET := redis-clone

SRC_DIR := src
BUILD_DIR := build

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/resp.c
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# $@ -> file to build
# $< -> first dependency
# $^ -> all the other files
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Meaning: "to obtain any '.o' file, compile is equivalent '.c'."
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@	

.PHONY: clean

clean: 
	rm -f $(TARGET) $(OBJ)
	
run: 
	./$(TARGET)
