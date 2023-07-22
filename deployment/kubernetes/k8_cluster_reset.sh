#!/bin/bash

kubeadm reset
systemctl stop kubelet
rm -rf /var/lib/cni/
rm -rf /var/lib/kubelet/*
rm -rf /etc/cni/
ifconfig cni0 down
ifconfig flannel.1 down

ip link set cni0 down && ip link set flannel.1 down
ip link delete cni0 && ip link delete flannel.1
systemctl restart containerd