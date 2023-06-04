SHELL = /bin/bash

run: matching_level1
	(time ./run.sh level1 > result.txt) 2>> res.txt
	@./answer_final.sh result level1

matching_level1: main_level1.cu imageUtil.cu
	@nvcc -w -O3 -o matching_level1 main_level1.cu imageUtil.cu -lm > /dev/null


.PHONY: run

level2: main_level2.c imageUtil.c
	gcc -w -O3 -o matching_level2 main_level2.c imageUtil.c -lm -mtune=native

clean:
	$(RM) matching
