folder=$1
for infolder in ${folder}/B*
do
    cnt=$(ls ${infolder} | wc -l)
    if [ $cnt != "0" ]
    then
        echo ${infolder}
    fi
done
