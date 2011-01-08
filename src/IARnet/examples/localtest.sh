#!/bin/bash
. ./start_server.sh local
sleep 2

./client --Ice.Config=local.conf
#gdb ./client

./stop_server.sh

