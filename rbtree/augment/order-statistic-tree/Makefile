# Makefile for xxxx
CC = gcc
CXX = g++
CFLAGS = -g
CFLAGS += -I ../../c/linux/
CXXFLAGS = -g -std=c++14
OBJS = $(patsubst %c, %o, $(wildcard *.c))
OBJS += $(patsubst %c, %o, ../../c/linux/rbtree.c)
OBJS += $(patsubst %cpp, %o, $(wildcard *.cpp))
HEADER = $(wildcard *.h)

Target = $(strip $(patsubst %c, %out, $(wildcard *_test.c)))

.PHONY: all
all: $(Target)

DEPS := $(OBJS:.o=.d)
-include $(DEPS)
CFLAGS += -MMD -MP
CXXFLAGS += -MMD -MP

%.o:%.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o:%.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(Target):$(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: test
test: ${Target}
	./${Target}

.PHONY: clean
clean:
	rm -rf *.o *.out $(Target) $(DEPS)
