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
DSPS_PS_ROOT_URI='127.0.0.1'
DSPS_PS_ROOT_PORT=8000
${bin} --scheduler-host=${DSPS_PS_ROOT_URI} \
    --scheduler-port=${DSPS_PS_ROOT_PORT} \
    --node_role='scheduler' &


# start servers
for ((i=0; i<${DSPS_NUM_SERVER}; ++i)); do
    ${bin} --scheduler-host=${DSPS_PS_ROOT_URI} \ 
        --scheduler-port=${DSPS_PS_ROOT_PORT} \ 
        --node_role='server' &
done

# start workers
for ((i=0; i<${DSPS_NUM_WORKER}; ++i)); do
    ${bin} --scheduler-host=${DSPS_PS_ROOT_URI} \ 
        --scheduler-port=${DSPS_PS_ROOT_PORT} \ 
        --node_role='worker' &
done

wait
