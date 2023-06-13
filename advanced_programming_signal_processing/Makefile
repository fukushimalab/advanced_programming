SHELL = /bin/bash
matching: level1 level2 level3 level4 level5 level6
.PHONY: level1 clean

level1: main_level1.cu imageUtil.cu
	nvcc -w -O3 -o matching_level1 main_level1.cu imageUtil.cu -lm

level2: main_level2.c imageUtil.c
	gcc -w -O3 -o matching_level2 main_level2.c imageUtil.c -lm -mtune=native

level3: main_level3.c imageUtil.c
	gcc -w -O3 -o matching_level3 main_level3.c imageUtil.c -lm -mtune=native

level4: main_level4.c imageUtil.c
	gcc -w -O3 -o matching_level4 main_level4.c imageUtil.c -lm -mtune=native

level5: main_level5.c imageUtil.c
	gcc -w -O3 -o matching_level5 main_level5.c imageUtil.c -lm -mtune=native

level6 : main_level6.c imageUtil.c
	gcc -w -O3 -o matching_level6 main_level6.c imageUtil.c -lm -mtune=native

clean:
	$(RM) matching
