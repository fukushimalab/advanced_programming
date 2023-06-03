#!/bin/sh
# imagemagickで何か画像処理をして，/imgprocにかきこみ，テンプレートマッチング
# 最終テストは，直下のforループを次に変更 for image in $1/final/*.ppm; do
for image in $1/final/*.ppm; do
    bname=`basename ${image}`
    name="imgproc/"$bname
    x=0    	#
    echo $name
    convert "${image}" "${name}"  # 何もしない画像処理
    rotation=0
    echo $bname:
    for template in $1/*.ppm; do
	echo `basename ${template}`
	if [ $x = 0 ]
	then
	    ./matching $name "${template}" $rotation 0.8 cpgw
	    x=1
	else
	    ./matching $name "${template}" $rotation 0.8 pgw
	fi
    done
    echo ""
done

convert -resize 50% $1/templ/airgun_women_syufu.ppm $1/airgun_women_syufu.ppm
convert -resize 50% $1/templ/kids_chuunibyou_girl.ppm $1/kids_chuunibyou_girl.ppm
convert -resize 50% $1/templ/mokuzai_hakobu.ppm $1/mokuzai_hakobu.ppm
convert -resize 50% $1/templ/ocean_beach_kinzokutanchi.ppm $1/ocean_beach_kinzokutanchi.ppm

for image in $1/final/*.ppm; do
    bname=`basename ${image}`
    name="imgproc/"$bname
    x=0    	#
    echo $name
    convert "${image}" "${name}"  # 何もしない画像処理
    rotation=0
    echo $bname:
	
    for template in $1/*.ppm; do
	echo `basename ${template}`
	if [ $x = 0 ]
	then
	    ./matching $name "${template}" $rotation 0.5 pgw
	    x=1
	else
	    ./matching $name "${template}" $rotation 0.5 pgw
	fi
    done
    echo ""
done

convert -resize 200% $1/templ/airgun_women_syufu.ppm $1/airgun_women_syufu.ppm
convert -resize 200% $1/templ/kids_chuunibyou_girl.ppm $1/kids_chuunibyou_girl.ppm
convert -resize 200% $1/templ/mokuzai_hakobu.ppm $1/mokuzai_hakobu.ppm
convert -resize 200% $1/templ/ocean_beach_kinzokutanchi.ppm $1/ocean_beach_kinzokutanchi.ppm
for image in $1/final/*.ppm; do
    bname=`basename ${image}`
    name="imgproc/"$bname
    x=0    	#
    echo $name
    convert "${image}" "${name}"  # 何もしない画像処理
    rotation=0
    echo $bname:
	
    for template in $1/*.ppm; do
	echo `basename ${template}`
	if [ $x = 0 ]
	then
	    ./matching $name "${template}" $rotation 0.02 pgw
	    x=1
	else
	    ./matching $name "${template}" $rotation 0.02 pgw
	fi
    done
    echo ""
    
done
    convert $1/templ/airgun_women_syufu.ppm $1/airgun_women_syufu.ppm
    convert $1/templ/kids_chuunibyou_girl.ppm $1/kids_chuunibyou_girl.ppm
    convert $1/templ/mokuzai_hakobu.ppm $1/mokuzai_hakobu.ppm
    convert $1/templ/ocean_beach_kinzokutanchi.ppm $1/ocean_beach_kinzokutanchi.ppm


wait