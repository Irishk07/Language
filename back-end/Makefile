CXX = g++

CPPFLAGS := -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat \
-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow \
-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn \
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef \
-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
-Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-protector \
-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=100000 -Wstack-usage=81920 -pie -fPIE -Werror=vla

SANITIZER_FLAGS := -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,$\
null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

CPPFLAGS += $(SANITIZER_FLAGS)


CPPSRC := main.cpp back_end.cpp back_end_x86.cpp back_end_bin.cpp onegin.cpp common_back.cpp
CPPSRC_COMMON := ../array.cpp ../common.cpp ../string_functions.cpp ../tree.cpp

CPPOBJ := $(CPPSRC:%.cpp=build/%.o) 
CPPOBJ_COMMON := $(CPPSRC_COMMON:../%.cpp=build/%.o)
HEADER_DEPENDS := $(CPPOBJ:%.o=%.d) $(CPPOBJ_COMMON:%.o=%.d)

.PHONY:  run_x86

.PHONY:
all: x86

.PHONY:
x86: back_end_prog

.PHONY: my_proc
my_proc: CPPFLAGS += -DMY_PROC
my_proc: back_end_prog

build_dir:
	@mkdir -p build

back_end_prog: build_dir $(CPPOBJ) $(CPPOBJ_COMMON)
	@$(CXX) $(CPPOBJ) $(CPPOBJ_COMMON) -o $@ $(CPPFLAGS)

$(CPPOBJ): build/%.o: %.cpp
	@$(CXX) $(CPPFLAGS) -MP -MMD -c $< -o $@

$(CPPOBJ_COMMON): build/%.o: ../%.cpp
	@$(CXX) $(CPPFLAGS) -MP -MMD -c $< -o $@

-include $(HEADER_DEPENDS)

.PHONY: run_x86
run_x86: x86
	@./back_end_prog ../tree.txt asm_x86.asm dump.html dump
	@chmod +x prog_x86.bin
	@./prog_x86.bin

.PHONY: run_my_proc
run_my_proc: my_proc
	@./back_end_prog ../tree.txt ../compilyator/asm/asm.txt dump.html dump

.PHONY: run_asm
run_asm:
	nasm -f elf64 asm_x86.asm -o asm_x86.o
	ld asm_x86.o -o asm_x86
	./asm_x86

clean:
	rm -f build/*.o
	rm -f build/*.d