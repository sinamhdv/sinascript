CC=gcc
CFLAGS=-Wall -g
OBJDIR=build

SRCS := $(shell find . -type f -name '*.c')
OBJS := $(addprefix $(OBJDIR)/,$(SRCS:%.c=%.o))
TARGET_BIN = lang

PACKAGES = utils parser
$(shell mkdir -p $(addprefix $(OBJDIR)/,$(PACKAGES)))

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET_BIN)
