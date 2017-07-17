#!/bin/bash
# set -x
if [ $# -lt 3 ]; then
    echo "usage: $0 num_servers num_workers bin [args..]"
    exit -1;
fi

export DSPS_NUM_SERVER=$1
shift
export DSPS_NUM_WORKER=$1
shift
bin=$1
shift
arg="$@"

# start the scheduler
export DSPS_PS_ROOT_URI='127.0.0.1'
export DSPS_PS_ROOT_PORT=8000
export DSPS_ROLE='SCHEDULER'
${bin} ${arg} &


# start servers
export DSPS_ROLE='SERVER'
for ((i=0; i<${DSPS_NUM_SERVER}; ++i)); do
    export HEAPPROFILE=./S${i}
    ${bin} ${arg} &
done

# start workers
export DSPS_ROLE='WORKER'
for ((i=0; i<${DSPS_NUM_WORKER}; ++i)); do
    export HEAPPROFILE=./W${i}
    ${bin} ${arg} &
done

wait
