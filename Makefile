CC=gcc
CFLAGS=-Wall -Wextra -g -DDEBUG
#CFLAGS=-Wall -Wextra -s
OBJDIR=build
SRCDIR=src

SRCS := $(shell find $(SRCDIR) -type f -name '*.c')
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
TARGET_BIN = lang

PACKAGES = utils parser types vm objects heap builtins
$(shell mkdir -p $(addprefix $(OBJDIR)/,$(PACKAGES)))

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET_BIN)
