ifeq ($(OS),Windows_NT)
    include Makefile.win
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        include Makefile.osx
    else
        $(error "Unknown OS: $(UNAME_S)")
        include Makefile.linux
    endif
endif
