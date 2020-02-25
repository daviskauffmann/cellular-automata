CC := gcc
CFLAGS := -ggdb -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter `pkg-config --cflags sdl2`
CPPFLAGS :=
LDFLAGS := `pkg-config --libs sdl2` -mconsole
LDLIBS :=

.PHONY: all
all: bin/ant bin/brain bin/empire bin/life bin/predator-and-prey bin/seeds

bin/%: obj/%.o
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@mkdir -p $(@D:obj%=dep%)
	$(CC) -c $< -o $@ -MMD -MF $(@:obj/%.o=dep/%.d) $(CFLAGS) $(CPPFLAGS)

-include $(SRC:src/%.c=dep/%.d)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf bin obj dep
