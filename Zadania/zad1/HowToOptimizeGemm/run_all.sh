#!/bin/bash

function run_all() {
    arr=("$@")
    for new in "${arr[@]}"; do
        if [ -z "$old" ]; then
            old="$new"
        else
            echo ">>>>> Running make run OLD=$old NEW=$new"
            make run OLD="$old" NEW="$new"
            old="$new"
        fi
    done
}

declare -a mmults_1x4=("MMult0" "MMult1" "MMult2" "MMult_1x4_3" "MMult_1x4_4" "MMult_1x4_5" "MMult_1x4_6" "MMult_1x4_7" "MMult_1x4_8" "MMult_1x4_9")
declare -a mmults_4x4=("MMult0" "MMult1" "MMult2" "MMult_4x4_3" "MMult_4x4_4" "MMult_4x4_5" "MMult_4x4_6" "MMult_4x4_7" "MMult_4x4_8" "MMult_4x4_9" "MMult_4x4_10" "MMult_4x4_11" "MMult_4x4_12" "MMult_4x4_13" "MMult_4x4_14" "MMult_4x4_15")

echo ">>>>> 1x4"
run_all "${mmults_1x4[@]}"
echo ">>>>> 4x4"
run_all "${mmults_4x4[@]}"
