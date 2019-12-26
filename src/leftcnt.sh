filename=$1
filename2=$2
cnt="0"
folder_q="/home/tang/tools/test/CPMS/src/mutator/source/shuju/"${filename2}
folder_firtst=$(pwd)

for((i=1;i<=20;i++))
do
    echo "PECWMA---------------------------"
    if [ ! -d "/home/tang/tools/test/CPMS/src/mutator/tt/Pecma/${filename}/Testcases" ];then
        mkdir /home/tang/tools/test/CPMS/src/mutator/tt/Pecma/${filename}/Testcases
    else
        tc_cnt=$( ls /home/tang/tools/test/CPMS/src/mutator/tt/Pecma/${filename}/Testcases | wc -w )
        if [ ${tc_cnt} -ne 0 ]
        then
            rm /home/tang/tools/test/CPMS/src/mutator/tt/Pecma/${filename}/Testcases/*
        fi
    fi
    /home/tang/tools/test/CPMS/src/mutator/tt/Pecma/${filename}/Scripts/TimetheRun.sh
    cd ${folder_firtst}
    ./resultcnt.sh ./tt/Pecma/${filename}/Scripts/
    #break
    echo "NORMAL--------------------------"
    cd ${folder_q}/Source
    rm  ${folder_q}/Testcases/*
    cp /home/tang/tools/test/CPMS/src/mutator/tt/Pecma/${filename}/Testcases/* ${folder_q}/Testcases/
    ./CMA_time.sh ${filename}
    cnt_tmp=$(ls ${folder_q}/Exe | wc -w)
    echo ${cnt_tmp}
    cnt=`expr ${cnt} + ${cnt_tmp}`
    sleep 2s

done
echo "can not kill mutant number are "`expr ${cnt} / 20`
cd ${folder_firtst}
./resultcnt.sh ./tt/Pecma/${filename}/Scripts/
