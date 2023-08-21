#!/bin/bash

# Usage ./create_instance.sh config.ini <INSTANCE_NAME>
ini_file="$1"

# Check if the file exists
if [[ ! -f "$ini_file" ]]; then
  echo "INI file '$ini_file' not found."
  exit 1
fi
VPC=$(awk -F "=" '/VPC/ {print $2}' $1)
ZONE=$(awk -F "=" '/ZONE/ {print $2}' $1)
AMI_ID=$(awk -F "=" '/AMI_ID/ {print $2}' $1)
SECURITY_GROUP=$(awk -F "=" '/SECURITY_GROUP/ {print $2}' $1)
INSTANCE_TYPE=$(awk -F "=" '/INSTANCE_TYPE/ {print $2}' $1)
INSTANCE_COUNT=$(awk -F "=" '/INSTANCE_COUNT/ {print $2}' $1)
INSTANCE_NAME=$2
KEY_NAME=$(awk -F "=" '/KEY_NAME/ {print $2}' $1)



progress_bar() {
  duration="$1"
  bar_length=75
  sleep_duration=$(echo "$duration / $bar_length" | bc)

  i=0
  while [ "$i" -le "$bar_length" ]; do
    printf "\r["

    j=0
    while [ "$j" -lt "$i" ]; do
      printf "="
      j=$((j+1))
    done

    printf ">"

    j=$((i+1))
    while [ "$j" -lt "$bar_length" ]; do
      printf " "
      j=$((j+1))
    done

    printf "] %d%%" "$((i*100/bar_length))"

    sleep "$sleep_duration"
    i=$((i+1))
  done

  printf "\n"
}

echo "Creating $INSTANCE_NAME server"
INSTANCE_ID=$(aws ec2 run-instances --image-id $AMI_ID --count $INSTANCE_COUNT --block-device-mappings '[{"DeviceName": "/dev/sda1","Ebs": {"VolumeSize": 30,"VolumeType": "gp2"}}]' --instance-type $INSTANCE_TYPE --key-name $KEY_NAME --security-group-ids $SECURITY_GROUP --subnet-id $ZONE --tag-specifications 'ResourceType=instance,Tags=[{Key=Name,Value='$INSTANCE_NAME'}]' --query 'Instances[0].InstanceId'  --output text)

progress_bar 75
echo "$INSTANCE_NAME Server Created Successfully!"

PUBLICIP=$(aws ec2 describe-instances --instance-ids $INSTANCE_ID --query 'Reservations[].Instances[].PublicIpAddress' | cut -d "[" -f2 | cut -d "]" -f1 | tr -d '" ')

echo "[$INSTANCE_NAME]" > /tmp/hosts
echo " " >> /tmp/hosts
echo "$PUBLICIP ansible_user=ubuntu" >> /tmp/hosts
