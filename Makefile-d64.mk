D64 = $(NAME).d64

C1541 ?= c1541.exe

# Unix or Windows
ifeq ($(shell echo),)
	CP = cp $1
else
	CP = copy $(subst /,\,$1)
endif

REMOVES += $(D64)

.PHONY: d64
d64: $(D64)

$(D64): $(NAME).c64
	$(C1541) -format "$(NAME)","01" d64 $(NAME).d64 -attach $(NAME).d64 -write $< $(NAME).prg
