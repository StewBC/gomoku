DSK = $(NAME).dsk

# For this one, see https://applecommander.github.io/
AC ?= ac.jar

# Unix or Windows
ifeq ($(shell echo),)
	CP = cp $1
else
	CP = copy $(subst /,\,$1)
endif

REMOVES += $(DSK)

.PHONY: dsk
dsk: $(DSK)

$(DSK): $(NAME).apple2
	$(call CP, apple2/template.dsk $@)
	java -jar $(AC) -p  $@ $(NAME).system sys < $(subst \,/,$(shell cl65 --print-target-path)/apple2/util/loader.system)
	java -jar $(AC) -as $@ $(NAME)        bin < $(NAME).apple2
