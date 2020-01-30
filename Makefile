CC := gcc
CFLAGS := -ggdb -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter `pkg-config --cflags sdl2`
CPPFLAGS :=
LDFLAGS := `pkg-config --libs sdl2` -mconsole
LDLIBS :=

.PHONY: all
all: bin/ant bin/brain bin/empire bin/life bin/predator-and-prey bin/seeds

bin/%: build/%.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CFLAGS) $(CPPFLAGS)

.PHONY: clean
clean:
	rm -rf bin build
