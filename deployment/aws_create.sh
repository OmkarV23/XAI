#!/bin/sh

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


INSTANCENAME=k8s-worker-2

echo "Creating $INSTANCENAME server"

INSTANCETYPE=t2.micro

AMI_ID=ami-0f8e81a3da6e2510a

ZONE=subnet-096c6d9d04ad74402

COUNTS=1

INSTANCE_ID=$(aws ec2 run-instances --image-id $AMI_ID --count $COUNTS --instance-type $INSTANCETYPE --key-name _Genetec_Key_ --security-group-ids sg-0cdb65a718991ffe3 --subnet-id $ZONE --tag-specifications 'ResourceType=instance,Tags=[{Key=Name,Value='$INSTANCENAME'}]' --query 'Instances[0].InstanceId'  --output text)

echo "$INSTANCE_ID"
progress_bar 75

echo "$INSTANCENAME Server Created Successfully!"


PUBLICIP=$(aws ec2 describe-instances --instance-ids $INSTANCE_ID --query 'Reservations[].Instances[].PublicIpAddress' | cut -d "[" -f2 | cut -d "]" -f1 | tr -d '" ')

PCLINE="[$INSTANCENAME]
$PUBLICIP ansible_user=ubuntu"

PHLINE="[$INSTANCENAME]\n\n$PUBLICIP ansible_user=ubuntu"

PATH1="/var/lib/jenkins/workspace/$INSTANCENAME"
PATH2="/root/.jenkins/workspace/$INSTANCENAME"

if [ "$(echo "$PWD")" = "$PATH1" ]; then
  echo "$PCLINE" > hosts
elif [ "$(echo "$PWD")" = "$PATH2" ]; then
  echo "$PCLINE" > hosts
else
  echo "$PHLINE" > hosts
fi

