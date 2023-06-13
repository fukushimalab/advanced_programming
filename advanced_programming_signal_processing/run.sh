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

    case $level in
    "level1")
        echo "Processing level1"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level2")
        echo "Processing level2"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level3")
        echo "Processing level3"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level4")
        echo "Processing level4"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level5")
        echo "Processing level5"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level6")
        echo "Processing level6"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level7")
        echo "Processing level7"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    "level8")
        echo "Processing level8"
        convert "${image}" "${name}"
        threshold=0.5
        ;;
    *)
        echo "Unknown level: $level"
        exit 1
        ;;
    esac
    rotation=0

    echo $bname:
    for template in $3/*.ppm; do
	echo `basename ${template}`
	if [ $x = 0 ]
	then
	    ./matching_${level} $name "${template}" $rotation $threshold cwp  # call the matching program corresponding to the level
	    x=1
	else
	    ./matching_${level} $name "${template}" $rotation $threshold wp  # call the matching program corresponding to the level
	fi
    done
    echo ""
}

level=$1  # receive the level from the command line
for image in ${level}/final/*.ppm; do
    process_image $image $level $level &
done

wait
