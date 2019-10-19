#! /bin/sh

PREFIX=${PREFIX:-t}
SEED=${SEED:-1571486961}

BUILD_FLAGS="-DDEBUG -DUSE_DICE" ./build.sh
./engine ${SEED} > /tmp/${PREFIX}_debug_dice_nodata

BUILD_FLAGS="-DUSE_DICE" ./build.sh
./engine ${SEED} > /tmp/${PREFIX}_nodebug_dice_nodata

BUILD_FLAGS="-DDEBUG" ./build.sh
./engine ${SEED} > /tmp/${PREFIX}_debug_nodice_nodata

BUILD_FLAGS="" ./build.sh
./engine ${SEED} > /tmp/${PREFIX}_nodebug_nodice_nodata

BUILD_FLAGS="-DDEBUG" ./build.sh
./engine ${SEED} 0 < ./engine_data5.txt > /tmp/${PREFIX}_debug_nodice_data

BUILD_FLAGS="" ./build.sh
./engine ${SEED} 0 < ./engine_data5.txt > /tmp/${PREFIX}_nodebug_nodice_data

if [ "${PREFIX}" = "t" ]; then
   if [ -f /tmp/t0_debug_dice_nodata ]; then
      diff -b /tmp/t0_debug_dice_nodata /tmp/t_debug_dice_nodata
   fi
   if [ -f /tmp/t0_nodebug_dice_nodata ]; then
      diff -b /tmp/t0_nodebug_dice_nodata /tmp/t_nodebug_dice_nodata
   fi
   if [ -f /tmp/t0_debug_nodice_nodata ]; then
      diff -b /tmp/t0_debug_nodice_nodata /tmp/t_debug_nodice_nodata
   fi
   if [ -f /tmp/t0_nodebug_nodice_nodata ]; then
      diff -b /tmp/t0_nodebug_nodice_nodata /tmp/t_nodebug_nodice_nodata
   fi
   if [ -f /tmp/t0_debug_nodice_data ]; then
      diff -b /tmp/t0_debug_nodice_data /tmp/t_debug_nodice_data
   fi
   if [ -f /tmp/t0_nodebug_nodice_data ]; then
      diff -b /tmp/t0_nodebug_nodice_data /tmp/t_nodebug_nodice_data
   fi
fi

exit 0
