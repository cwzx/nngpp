#!/usr/bin/env bash
set -euo pipefail

MODE="$1"

pkill local_lat || true
pkill local_thr || true

for SIZE in 8 64 512 4096 32768 262144 1000000
do
    echo
    echo Message size of $SIZE bytes:

    bin/perf/local_lat "$MODE://127.0.0.1:5555" "$SIZE" 1000 &
    pid=$!
    sleep 1
    bin/perf/remote_lat  "$MODE://127.0.0.1:5555" "$SIZE" 1000
    wait $pid

    bin/perf/local_thr "$MODE://127.0.0.1:5555" "$SIZE" 1000 &
    pid=$!
    sleep 1
    bin/perf/remote_thr  "$MODE://127.0.0.1:5555" "$SIZE" 1000
    wait $pid
done

pkill local_lat || true
pkill local_thr || true
