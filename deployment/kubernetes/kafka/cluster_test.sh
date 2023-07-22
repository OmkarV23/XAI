#!/bin/bash

echo "hello Kafka" | kafkacat -P -b 172.31.3.62:32400 -t test
kafkacat -C -b 172.31.3.62:32400 -t test