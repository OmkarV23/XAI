#!/bin/bash

echo "Starting Kafka server"
docker-compose -f docker-compose.yml up -d

sleep 10

# Path to the JSON file
json_file="camera_map.json"

# Read and parse the JSON file using jq
data=$(jq '.' "$json_file")

red='\033[0;31m'
green='\033[0;32m'

# Loop through each key in the Camra_map object
keys=$(echo "$data" | jq -r '.Camera_map | keys[]')
for key in $keys; do
    # Extract the camera IP for the current key
    camera_ip=$(echo "$data" | jq -r ".Camera_map[\"$key\"].camera_ip")
    
    
    echo -e "Creating topic: $key for Camera IP: $camera_ip"
    docker exec -it broker kafka-topics --bootstrap-server 75.204.78.27:9092 \
            --replication-factor 1 --partitions 1 --create --topic $key
    echo
done
