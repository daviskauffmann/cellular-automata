CC := gcc
CFLAGS := -ggdb -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter `pkg-config --cflags sdl2`
LDFLAGS := `pkg-config --libs sdl2`

SRC	:= src
BUILD := build
BIN	:= bin

INCLUDE :=
LIB :=
LIBRARIES :=

.PHONY: all
all: $(BIN)/ant $(BIN)/brain $(BIN)/empire $(BIN)/life $(BIN)/predator-and-prey $(BIN)/seeds

$(BIN)/%: $(BUILD)/%.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS) $(LIB) $(LIBRARIES)

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE)

-include $(DEPENDENCIES)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)
