#!/usr/bin/env bash

# Get and enter the directory of the script
sbin=`dirname $0`
sbin=`cd $sbin; pwd`

# Get the home 
export OT_HOME=`cd $sbin/../; pwd`

# Get the converter
converter="${OT_HOME}/bin/ot-utility"

# Get the benchmark
benchmark=`cd $OT_HOME/benchmark/; pwd`

# Scan each .tau2015
for f in `find $benchmark -name *.tau2015`
do
  ckt=`echo $(basename "$f" .tau2015) | sed -e 's/-/_/g'`
  echo "Processing circuit ${benchmark}/${ckt} ..."
  cd "$benchmark/${ckt}/"; ${converter} --tau15-to-shell ${ckt}.tau2015 ${ckt}.shell;
  echo "done"
done
