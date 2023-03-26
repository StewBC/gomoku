PO = $(NAME).po

CA ?= cadius

# Unix or Windows
ifeq ($(shell echo),)
	CP = cp $1
	MV = mv
else
	CP = copy $(subst /,\,$1)
	MV = ren
endif

REMOVES += $(PO)

.PHONY: po
po: $(PO)

$(NAME).system:
	CP $(subst \,/,$(shell cl65 --print-target-path)/apple2/util/loader.system) $(NAME).system#FF2000

$(PO): $(NAME).apple2 $(NAME).system
	$(call CP, apple2/template.po $@)
	$(MV) $(NAME).apple2 $(NAME)#060803
	$(CA) addfile $(NAME).po /gomoku $(NAME).system#FF2000
	$(CA) addfile $(NAME).po /gomoku $(NAME)#060803
	$(RM) $(NAME).system#FF2000
	$(RM) $(NAME)#060803