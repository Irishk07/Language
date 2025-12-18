.PHONY: all
all: run

.PHONY: run
run:
	cd front-end && make && ./front_end_prog dump.html dump sq_solver.txt ../tree.txt
	cd middle-end && make && ./middle_end_prog dump.html dump ../tree.txt
	cd back-end && make && ./back_end_prog ../tree.txt ../compilyator/asm/asm.txt dump.html dump
	cd compilyator/asm && make && ./asm_prog asm.txt
	cd compilyator/processor && make && ./proc_prog