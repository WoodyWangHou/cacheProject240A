#!/usr/bin/env bash
# apt update 
# apt install valgrind
make clean
make

# ./unit_test.sh

FILES="../traces/*"

INTEL_I='256:1:2'
INTEL_D='256:1:2'
INTEL_L2='512:8:10'
INTEL_BLOCK='64'
INTEL_MEM='100'

ARM_I='128:2:2'
ARM_D='128:4:2'
ARM_L2='256:8:10'
ARM_BLOCK='64'
ARM_MEM='100'

MIPS_I='128:2:2'
MIPS_D='64:4:2'
MIPS_L2='128:8:50'
MIPS_BLOCK='128'
MIPS_MEM='100'

ALPHA_I='512:2:2'
ALPHA_D='256:4:2'
ALPHA_L2='16384:8:50'
ALPHA_BLOCK='64'
ALPHA_MEM='100'

BTCMINER_I='0:0:0'
BTCMINER_D='0:0:0'
BTCMINER_L2='8:1:50'
BTCMINER_BLOCK='128'
BTCMINER_MEM='100'

INTEL_DIR='./results/intel'
ARM_DIR='./results/arm'
MIPS_DIR='./results/mips'
ALPHA_DIR='./results/alpha'
BTCMINER_DIR='./results/btcminer_DIR'

mkdir -p $INTEL_DIR
mkdir -p $ARM_DIR
mkdir -p $MIPS_DIR
mkdir -p $BTCMINER_DIR
mkdir -p $ALPHA_DIR

OPTS=''
# run experiments on Intel
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | $OPTS ./cache --icache=$INTEL_I --dcache=$INTEL_D --l2cache=$INTEL_L2 --blocksize=$INTEL_BLOCK --memspeed=$INTEL_MEM --inclusive > "$INTEL_DIR/$TESTRESULT"
done

# # run experiments on ARM
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | $OPTS ./cache --icache=$ARM_I --dcache=$ARM_D --l2cache=$ARM_L2 --blocksize=$ARM_BLOCK --memspeed=$ARM_MEM > "$ARM_DIR/$TESTRESULT"
done

# run experiments on MIPS
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | $OPTS ./cache --icache=$MIPS_I --dcache=$MIPS_D --l2cache=$MIPS_L2 --blocksize=$MIPS_BLOCK --memspeed=$MIPS_MEM --inclusive > "$MIPS_DIR/$TESTRESULT"
done

# run experiments on Alpha
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | $OPTS ./cache --icache=$ALPHA_I --dcache=$ALPHA_D --l2cache=$ALPHA_L2 --blocksize=$ALPHA_BLOCK --memspeed=$ALPHA_MEM --inclusive > "$ALPHA_DIR/$TESTRESULT"
done

# run experiments on bitcoin miner
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | $OPTS ./cache --icache=$BTCMINER_I --dcache=$BTCMINER_D --l2cache=$BTCMINER_L2 --blocksize=$BTCMINER_BLOCK --memspeed=$BTCMINER_MEM > "$BTCMINER_DIR/$TESTRESULT"
done
