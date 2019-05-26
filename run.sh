#!/bin/sh
# imagemagickで何か画像処理をして，/imgprocにかきこみ，テンプレートマッチング
for image in $1/test/*.ppm; do
    bname=`basename ${image}`
    name="imgproc/"$bname
    x=0    	#
    echo $name
    convert "${image}" $name  # 何もしない画像処理
#   convert -blur 2x6 "${image}" "${name}"
#    convert -median 3 "${image}" "${name}"
    rotation=0
    echo $bname:
    for template in $1/*.ppm; do
	echo `basename ${template}`
	if [ $x = 0 ]
	then
	    ./matching $name "${template}" rotation 0.5 cp
	    x=1
	else
	    ./matching $name "${template}" rotation 0.5 p
	fi
    done
    echo ""
done
wait
