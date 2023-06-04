#!/bin/sh 
# tolerance error
POSITION_TOLERANCE_ERROR=0
WIDHT_TOLERANCE_ERROR=0
HIGHT_TOLERANCE_ERROR=0
ROTATION_TOLERANCE_ERROR=0

correct=0
sum=0
not_exist=0

debug=0
if [ $# -ge 3 ]; then
	if [ $3 = "d" ]; then
		debug=1
	fi
fi

for result in $1/*.txt; do
	basename ${result}
	answer=$2/final/`basename ${result}` #ここをtest->finalに変えただけ．
	if [ -e ${answer} ]; then
		sum=`echo ${sum}+1 | bc`

		result_template_name=`cat ${result} | awk '{print $1}'`
		result_x=`cat ${result} | awk '{print $2}'`
		result_y=`cat ${result} | awk '{print $3}'`
		result_width=`cat ${result} | awk '{print $4}'`
		result_hight=`cat ${result} | awk '{print $5}'`
		result_rotation=`cat ${result} | awk '{print $6}'`
		
		answer_template_name=`cat ${answer} | awk '{print $1}'`
		answer_x=`cat ${answer} | awk '{print $2}'`
		answer_y=`cat ${answer} | awk '{print $3}'`
		answer_width=`cat ${answer} | awk '{print $4}'`
		answer_hight=`cat ${answer} | awk '{print $5}'`
		answer_rotation=`cat ${answer} | awk '{print $6}'`

		if [ -z ${result_template_name} ] || [ -z "${result_x}" ] || [ -z "${result_y}" ] || [ -z "${result_width}" ] || [ -z "${result_hight}" ] || [ -z "${result_rotation}" ];  then
			echo "[NOT CORRECT (IGNORE RESULT FILE)]"
			continue
		fi
		if [ ${result_template_name} = ${answer_template_name} ]; then
			x=`expr ${result_x} - ${answer_x}`
			y=`expr ${result_y} - ${answer_y}`
			width=`expr ${result_width} - ${answer_width}`
			hight=`expr ${result_hight} - ${answer_hight}`
			rotation=`expr ${result_rotation} - ${answer_rotation}`

			x=`echo "sqrt(${x}^2)" | bc`
			y=`echo "sqrt(${y}^2)" | bc`
			width=`echo "sqrt(${width}^2)" | bc`
			hight=`echo "sqrt(${hight}^2)" | bc`
			rotation=`echo "sqrt(${rotation}^2)" | bc`

			if [ ${debug} = 1 ]; then
				echo "debug info: "${x} ${y} ${width} ${hight} ${rotation}
			fi

			if test ${x} -le ${POSITION_TOLERANCE_ERROR} && \
			   test ${y} -le ${POSITION_TOLERANCE_ERROR} && \
			   test ${width} -le ${WIDHT_TOLERANCE_ERROR} && \
			   test ${hight} -le ${HIGHT_TOLERANCE_ERROR} && \
			   test ${rotation} -le ${ROTATION_TOLERANCE_ERROR}; then

				echo "[CORRECT]"
				correct=`echo ${correct}+1 | bc`
			else
				echo "[NOT CORRECT]"
			fi
		else
			echo "[NOT CORRECT (NOT MATCH TEMPLATE)]"
		fi
	else
		echo "[ANSWER FILE NOT EXIST]"
		not_exist=`echo ${not_exist}+1 | bc`
	fi
done

echo ""
echo "=========================="
echo "SUMMARY"
echo "CORRECT RATE: "${correct}"/"${sum}
echo "ANSWER FILE NOT EXIST: "${not_exist}
echo "=========================="
