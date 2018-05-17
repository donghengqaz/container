
CC := gcc
SRC := $(wildcard ./*.c)
HDF := $(wildcard ./*.h)

main: $(SRC) $(HDF)
	@$(CC) $< -o $@

.phony: clean
clean:
	@rm main
