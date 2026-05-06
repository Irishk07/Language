.PHONY: all
all: run_x86

.PHONY: run_my_proc
run_my_proc:
	cd front-end && make && ./front_end_prog dump.html dump factorial.txt ../tree.txt
	cd middle-end && make && ./middle_end_prog dump.html dump ../tree.txt
	cd back-end && make clean && make my_proc && make run_my_proc
	cd compilyator/asm && make && ./asm_prog asm.txt
	cd compilyator/processor && make && ./proc_prog

.PHONY: run_x86
run_x86:
	cd front-end && make && ./front_end_prog dump.html dump factorial.txt ../tree.txt
	cd middle-end && make && ./middle_end_prog dump.html dump ../tree.txt
	cd back-end && make && make run_x86