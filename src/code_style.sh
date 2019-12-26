file_A=$1
folder_B="../MutationT"
folder_C="../MutationP/"
rm output.txt
for file_b in ${folder_B}/*.c;do
    temp_file_b=` basename ${file_b} .c `
    indent -kr -i8 -nbap -sob -l360 -nce -bl ${file_b} -o ../MutationP/${temp_file_b}.c
done

for file_b in ${folder_C}/*.c;do
    cnt=`diff -b ${file_b} ./${file_A}  | wc -l `
    if [[ ${cnt} -ne 4 ]];then
        echo ${file_b} >> output.txt
        diff -b ${file_b} ./${file_A} >> output.txt
    fi
done
