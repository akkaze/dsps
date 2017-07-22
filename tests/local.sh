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

# start the scheduler
DSPS_PS_ROOT_URI='10.170.124.249'
DSPS_PS_ROOT_PORT=8000
${bin} --scheduler_host=${DSPS_PS_ROOT_URI} \
    --scheduler_port=${DSPS_PS_ROOT_PORT} \
    --node_role='SCHEDULER' &

sleep 1
# start servers
for ((i=0; i<${DSPS_NUM_SERVER}; ++i)); do
    ${bin} --scheduler_host=${DSPS_PS_ROOT_URI} --scheduler_port=${DSPS_PS_ROOT_PORT} --node_role='SERVER' &
done

# start workers
for ((i=0; i<${DSPS_NUM_WORKER}; ++i)); do
    ${bin} --scheduler_host=${DSPS_PS_ROOT_URI} --scheduler_port=${DSPS_PS_ROOT_PORT} --node_role='WORKER' &
done

wait
