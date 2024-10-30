#!/bin/bash

SERVER_EXEC="./webserv"
CONFIG_FILE="configs/default.conf"
HURL_TEST_DIR="./tests"

echo "Compiling server..."
make all
if [ $? -ne 0 ]; then
  echo "Compilation failed. Exiting."
  exit 1
fi
echo "Compilation successful."

echo "Starting the server..."
$SERVER_EXEC $CONFIG_FILE &
SERVER_PID=$!
sleep 1

echo "Running Hurl tests..."
for hurl_file in "$HURL_TEST_DIR"/*.hurl; do
  echo "Testing $hurl_file..."
  hurl "$hurl_file"
  
  if [ $? -ne 0 ]; then
    echo "Test failed for $hurl_file"
  else
    echo "Test passed for $hurl_file"
  fi
done

echo "Stopping the server..."
kill $SERVER_PID

echo "All tests completed."

