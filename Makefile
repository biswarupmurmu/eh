CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = array.c draw.c editor.c filehandler.c keypresshandler.c main.c terminal.c

BUILD_DIR = build
TARGET = $(BUILD_DIR)/eh

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

.PHONY: all release clean

all: $(TARGET)

release: CFLAGS = -Wall -Wextra -O2
release: clean all

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
