matching: level1 level2

level1: main_level1.cu imageUtil.cu
	nvcc -w -O3 -o matching_level1 main_level1.cu imageUtil.cu -lm

level2: main_level2.c imageUtil.c
	gcc -w -O3 -o matching_level2 main_level2.c imageUtil.c -lm -mtune=native

clean:
	$(RM) matching
