ifeq ($(DEBUG),1)
	debug := -ggdb3
endif

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

wrapper_name := fastadedupe_wrap.cxx
out_one := fastadedupe.o
out_two := fastadedupe_wrap.o
out_deps :=  immx.o seqmap.o concurrent.o
out_assemble := _libfastadedupe.so

SRC_DIRS := ./src
SRCS := ./src/fastadedupe.cpp ./src/immx.cpp ./src/seqmap.cpp ./src/concurrent.cpp

OBJS := $(out_assemble) $(out_one) $(out_two)

INC_DIRS := ./include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPP := $(COMPILER)
CPPFLAGS := -std=c++14 -c -mavx2 $(debug) -fPIC  $(INC_FLAGS)
CPPFLAGS_WRAPPER := -std=c++14 -c -mavx2 -fPIC -I$(PYTHON_HEADERS)

all: clean build_one build_assemble post_clean
install: install_t ldc

ldc:
	ldconfig -n $(DESTDIR)$(PREFIX)/lib/

install_t:
	mv ./$(OBJS) $(DESTDIR)$(PREFIX)/lib/
	mkdir -p $(DESTDIR)$(PREFIX)/include/fastadedupe
	cp $(INC_DIRS)/* $(DESTDIR)$(PREFIX)/include/fastadedupe

build_one: 
	$(CPP) $(CPPFLAGS) $(SRCS)
build_two:
	$(CPP) $(CPPFLAGS_WRAPPER) $(wrapper_name)
build_assemble:
	$(CPP) $(debug) -shared $(out_one) $(out_deps) -o $(out_assemble)

post_clean:
	rm -f $(out_deps) $(out_one)

clean:
	if [ -d "${OBJS}" ]; then \
        rm -r ${OBJS}; \
    fi \

