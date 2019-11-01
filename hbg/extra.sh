#! /bin/sh

# largest known values for thresh1 and thresh2 are 308 and 807, respectively (seed=3896)
# continue using "SEED=-1 /bin/sh ./extra.sh 308 807"

SEED=${SEED:--2}

echo "nargs: $#"
echo "args: $*"

if [ $# -lt 2 ]; then
   echo "error: expected two integer values as input"
   exit 1
fi

thresh1=$1
shift
thresh2=$1
shift

count=3896

while true; do
   if [ $SEED -eq -1 ]; then
      ./engine ${count} 0 < engine_data.txt > /tmp/t
   else
      ./engine ${SEED} 0 < engine_data.txt > /tmp/t
   fi
   head -1 /tmp/t
   egrep -q "^3|^8" /tmp/t
   result=$?
   if [ $result -eq 0 ]; then
     val1=`egrep "^3" /tmp/t | sort | tail -1 | awk '{ print $1 }'`
     result1=$?
     val2=`egrep "^8" /tmp/t | sort | tail -1 | awk '{ print $1 }'`
     result2=$?
     echo "result1:"$result1" val1:"$val1" thresh1:"$thresh1
     echo "result2:"$result2" val2:"$val2" thresh2:"$thresh2
     if [ ! -z $val1 ]; then
        if [ $val1 -gt $thresh1 ]; then
           break
        fi
     fi
     if [ ! -z $val2 ]; then
        if [ $val2 -gt $thresh2 ]; then
           break
        fi
     fi
   fi
   if [ $SEED -ne -1 ]; then
      sleep 0.5s
      SEED=-2
   else
      count=`expr $count + 1`
   fi
done

head -1 /tmp/t

exit 0
