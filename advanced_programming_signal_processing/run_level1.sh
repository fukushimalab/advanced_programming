# 想定される実行:
# time sh run_level1.sh
# 注意： level1は cuda を使用する特別なプログラムなため、このシェルスクリプトを実行してください
for image in level1/final/*.ppm; do 
	bname=`basename ${image}`; 
	name="imgproc/"${bname}; 
	convert ${image} ${name}; 
done
time (./matching_level1 > result.txt) 2>> result.txt
sh answer_final.sh result level1