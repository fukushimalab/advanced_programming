SHELL = /bin/bash
.PHONY: level1 clean

level1: matching_level1
	@for image in level1/final/*.ppm; do \
		bname=`basename $${image}`; \
		name="imgproc/"$${bname}; \
		convert $${image} $${name}; \
	done
	@time (./matching_level1 level1 > result.txt) 2>> result.txt
	@./answer_final.sh result level1

matching_level1: main_level1.cu imageUtil.cu
	nvcc -w -O3 -o matching_level1 main_level1.cu imageUtil.cu -lm

level2: main_level2.c imageUtil.c
	gcc -w -O3 -o matching_level2 main_level2.c imageUtil.c -lm -mtune=native

clean:
	$(RM) matching
