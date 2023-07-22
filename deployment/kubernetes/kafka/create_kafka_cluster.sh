#!/bin/bash

kubectl apply -f persistent-volume/kafka-storage.yml

if [[ $(helm repo list | grep rhcharts) ]]; then
    echo "Repo already added"
else
    helm repo add rhcharts https://ricardo-aires.github.io/helm-charts/
fi

helm install kafka -f helm-chart/values.yaml rhcharts/kafka