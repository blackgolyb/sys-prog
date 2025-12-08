#!/bin/bash

# Start the program in background
echo "Starting philosopher program..."
./build/main &
PID=$!

# Give it time to start
sleep 1

echo ""
echo "=== Testing SIGUSR1 reminders ==="
echo ""

for i in {1..5}; do
    echo "Sending reminder #$i (SIGUSR1)..."
    kill -SIGUSR1 $PID
    sleep 1
done

wait $PID
EXIT_CODE=$?

echo ""
echo "Program exited with code: $EXIT_CODE"
echo ""

echo "=== Testing SIGINT (Ctrl+C) scenario ==="
echo ""
echo "Starting philosopher program again..."
./build/main &
PID=$!

sleep 1

echo "Sending 2 reminders..."
kill -SIGUSR1 $PID
sleep 1
kill -SIGUSR1 $PID
sleep 1

echo ""
echo "Sending SIGINT (simulating Ctrl+C)..."
kill -SIGINT $PID

wait $PID
EXIT_CODE=$?

echo ""
echo "Program exited with code: $EXIT_CODE"
echo ""
