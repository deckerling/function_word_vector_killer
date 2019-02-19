CFLAGS := -g -Wall
SRCS := $(wildcard src/*.cc)

function_word_vector_killer: $(SRCS) src/function_word_vector_killer.h
	g++ $(SRCS) src/function_word_vector_killer.h -o function_word_vector_killer $(CFLAGS)

clean:
	rm -rf function_word_vector_killer
