CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -O2
TARGET   = notifies

all: $(TARGET)

$(TARGET): notifies.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: all clean