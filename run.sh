#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <task_number> <array_size> <num_tests>"
    exit 1
fi

task_num=$1
array_size=$2
num_tests=$3
threads_list=(1 2 4 8 16)

# Получаем список доступных ядер (0,1,2,...)
all_cores=$(seq -s ',' 0 $(($(nproc --all) - 1)))
echo "Available CPU cores: $all_cores"

echo "Task $task_num | Array size: $array_size | Tests: $num_tests"
echo "--------------------------------------------------"
echo "Threads | Avg Time (sec) | CPU Cores Used"
echo "--------|---------------|----------------"

for threads in "${threads_list[@]}"; do
    # Выбираем первые $threads ядер
    cores=$(cut -d ',' -f 1-$threads <<< "$all_cores")
    
    total_time=0
    for ((test=1; test<=num_tests; test++)); do
        export OMP_NUM_THREADS=$threads
        export OMP_PROC_BIND=TRUE
        export OMP_PLACES=cores
        
        if [ "$task_num" -eq 4 ]; then
            rows=$((array_size / 1000))
            cols=1000
            output=$(taskset -c "$cores" ./"$task_num" "$rows" "$cols" "$threads")
        else
            output=$(taskset -c "$cores" ./"$task_num" "$array_size" "$threads")
        fi
        
        time=$(echo "$output" | grep "Time:" | awk '{print $2}')
        total_time=$(echo "$total_time + $time" | bc)
    done
    
    avg_time=$(echo "scale=6; $total_time / $num_tests" | bc)
    printf "%7d | %13.6f | %s\n" "$threads" "$avg_time" "$cores"
done
