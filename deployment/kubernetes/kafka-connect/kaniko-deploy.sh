#!/bin/bash

kubectl apply -f kaniko-pod.yaml
sleep 240
kubectl delete pod --field-selector=status.phase==Succeeded
kubectl apply -f kafka-mongodb-connector.yaml