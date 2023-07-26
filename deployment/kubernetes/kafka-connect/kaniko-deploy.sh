#!/bin/bash

kubectl apply -f kaniko-pod.yaml
sleep 30
kubectl logs kaniko  --all-containers --follow
kubectl apply -f kafka-mongodb-connector.yaml