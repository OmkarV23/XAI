#!/bin/bash


if ! command -v helm &> /dev/null
then
    curl https://baltocdn.com/helm/signing.asc | gpg --dearmor | sudo tee /usr/share/keyrings/helm.gpg > /dev/null
    sudo apt-get install apt-transport-https --yes
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/helm.gpg] https://baltocdn.com/helm/stable/debian/ all main" | sudo tee /etc/apt/sources.list.d/helm-stable-debian.list
    sudo apt-get update -y
    sudo apt-get install helm -y
fi

if [[ $(helm repo list | grep rhcharts) ]]; then
    echo "Repo already added"
else
    helm repo add rhcharts https://ricardo-aires.github.io/helm-charts/
fi

kubectl apply -f kafka-pv.yml
kubectl apply -f zookeeper-pv.yml
helm install kafka -f kafka-config-values.yaml ./kafka