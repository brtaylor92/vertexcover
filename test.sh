#! /bin/zsh

make
for i in {0..4};
do
  echo $i":"
  time ./proj < t$i;
done;
