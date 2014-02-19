STUDENT_ID=2208496

DIR=bash-4.2
STR=execute
NUM_FILES=10

build:
	gcc -Wall -g finder.c -o finder

find:
	/bin/bash finder.sh $(DIR) $(STR) $(NUM_FILES)
t: build
	./finder $(DIR) $(STR) $(NUM_FILES)
test: build
	/bin/bash finder.sh $(DIR) $(STR) $(NUM_FILES) > tmp1
	./finder $(DIR) $(STR) $(NUM_FILES) > tmp2
	-diff tmp1 tmp2
	rm -f tmp1 tmp2

pipe: pipe.c
	gcc -Wall -g pipe.c -o pipe

clean:
	rm -f finder pipe tmp1 tmp2

tar:
	make clean
	mkdir $(STUDENT_ID)-ipc-lab
	cp -r bash-4.2 Makefile finder.sh finder.c  report.txt $(STUDENT_ID)-ipc-lab
	tar cvzf $(STUDENT_ID)-ipc-lab.tar.gz $(STUDENT_ID)-ipc-lab
	rm -rf $(STUDENT_ID)-ipc-lab
