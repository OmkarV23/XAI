#!/bin/bash

kubectl apply -f persistent-volume.yml
kubectl apply -f pvc.yml
kubectl apply -f statefulset.yml
kubectl apply -f service.yml