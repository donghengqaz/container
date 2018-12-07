
CC := gcc

INC_DIRS := ../include ../port/include .
INC_FLAGS := $(foreach dir,$(INC_DIRS),-I $(dir))
INCS := $(foreach dir,$(INC_DIRS),$(wildcard $(dir)/*.h))

SRC_DIRS := ../src .
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

FLAGS := -ggdb

main: $(SRCS) $(INCS)
	$(CC) $(SRCS) -o $@ $(INC_FLAGS) $(FLAGS)

.phony: clean info
clean:
	@rm main

info:
	@echo $(INC_FLAGS) $(INCS) $(SRCS)
