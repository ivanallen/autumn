#!/bin/bash
for filename in `find ./src | egrep '\.cc'`; 
do 
  gcov -o ./objs $filename >/dev/null; 
done
