#!/bin/bash

# Pull required container images
kubeadm config images pull

# Initialize the Kubernetes cluster
kubeadm init --pod-network-cidr=10.244.0.0/16

export KUBECONFIG=/etc/kubernetes/admin.conf

# Install Flannel plugin
kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml

# Get join token
kubeadm token create --print-join-command
