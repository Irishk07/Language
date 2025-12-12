.PHONY: all
all: run

.PHONY: run
run:
	cd front-end && make && ./front_end_prog dump.html dump programm.txt ../tree.txt
	cd middle-end && make && ./middle_end_prog dump.html dump ../tree.txt