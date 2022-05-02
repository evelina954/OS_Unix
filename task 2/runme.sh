#!/bin/bash
rm -f somefile.lck stats results.txt
make -s lock

count=10
pids=()
for i in {1..10}; do
	./lock &
	pids+=($!)
done
sleep 5m

completedTasks=0
for pid in ${pids[@]}
do
	kill -SIGINT $pid
	if [[ $? -eq 0 ]]
	then
		((completedTasks++))
	fi
done

result="results.txt"
echo "Expected completed tasks:" $count >>"$result"
echo "Completed tasks in fact:" $completedTasks >>"$result"

echo "Expected str in stat file:" $count >>"$result"
echo "Str in stat file in fact:" $(cat stats | wc -l) >>"$result"

echo "Expected locks per task: > 0 for all tasks" >>"$result"
echo "In fact:" $(cut -d: -f2 stats | xargs) >>"$result"
