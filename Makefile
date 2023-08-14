CC       := gcc
OUT      := output

SRCS     := chan/chan.c
CFLAGS   := -g -Wall -std=c99 -Ichan -lpthread
EXAMPLES := $(basename $(wildcard examples/*.c))

.PHONY: all
all: $(OUT) $(EXAMPLES)

.PHONY: clean
clean:
	rm -rf $(OUT)

.PHONY: $(OUT)
$(OUT):
	mkdir -p $@

define build_rule
.PHONY: $1
$1: $1.c
	$$(CC) -o $$(OUT)/$$(notdir $$@) $$^ $$(SRCS) $$(CFLAGS)
endef

$(foreach target,$(EXAMPLES),$(eval $(call build_rule,$(target))))
