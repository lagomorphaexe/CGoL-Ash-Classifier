#!/bin/bash

N_RUNS=0;#____#
N_THIS=$(($N_RUNS+1))

if ! [ -z "$1" ]
then
  N_THIS="$1"
  echo N_THIS updated to $N_THIS.
fi

GOL_RNG_FILE="search-files/unprocessed.rle"
GOL_ADV_FILE="search-files/COUNT_${N_THIS}.rle"

echo "-------------------------GENERATING RANDOM PATTERN FILE-------------------------"
TIME_1=$(date +%s)
./genGrid $GOL_RNG_FILE 30000 20000
TIME_2=$(date +%s)
echo "(->$GOL_RNG_FILE), runtime: $(($TIME_2 - $TIME_1))s"
echo "-------------------------ADVANCING RANDOM PATTERN FILE-------------------------"
TIME_1=$(date +%s)
bgolly -m 100000 -o $GOL_ADV_FILE $GOL_RNG_FILE
TIME_2=$(date +%s)
echo ", runtime: $(($TIME_2 - $TIME_1))s"
echo "-------------------------ANALYZING ADVANCED FILE-------------------------"
TIME_1=$(date +%s)
./analyzeRLE -c -d $GOL_ADV_FILE
TIME_2=$(date +%s)
echo "runtime: $(($TIME_2 - $TIME_1))s"
echo "-------------------------MAIN SCRIPT FINISHED----------------------------"

sed -i "/#____#$/s/=.*#/=$N_THIS;#____#/" ${0}

notify-send "Analysis Complete at $GOL_ADV_FILE"
mpg123 -q ball.mp3
