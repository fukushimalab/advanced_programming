#!/bin/sh
# imagemagickで何か画像処理をして，/imgprocにかきこみ，テンプレートマッチング
# 最終テストは，直下のforループを次に変更 for image in $1/final/*.ppm; do

process_image() {
    image=$1
    level=$2
    bname=`basename ${image}`
    name="imgproc/"$bname
    x=0    	#
    echo $name
    convert "${image}" "${name}"  # 何もしない画像処理
#   convert -blur 2x6 "${image}" "${name}"
#   convert -median 3 "${image}" "${name}"
#   convert -auto-level "${image}" "${name}"
#   convert -equalize "${image}" "${name}"
    rotation=0
    echo $bname:
    for template in $3/*.ppm; do
	echo `basename ${template}`
	if [ $x = 0 ]
	then
	    ./matching_${level} $name "${template}" $rotation 0.5 cwp  # call the matching program corresponding to the level
	    x=1
	else
	    ./matching_${level} $name "${template}" $rotation 0.5 wp  # call the matching program corresponding to the level
	fi
    done
    echo ""
}

level=$1  # receive the level from the command line
for image in ${level}/final/*.ppm; do
    process_image $image $level $level &
done

wait
