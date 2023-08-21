# Deploying EC2 instance and configuring Kafka K8 cluster
## EC2 deployment
    cd aws
    ./create_instance config.ini <instance-name>

## Installing Kubernetes and configuring
    ansible-playbook install_k8.yaml --private-key="_Genetec_Key_.pem" -e host=<instance-name>

## Configuring the cluster

#### On the controller node

    cd kuberenetes
    ./control_plane_init.sh (Update the docker hub credentials)
    kubectl label nodes <controller-node-ip>  type=cloud provider=aws service=ec2 region=us-west-1 availability_zone=us-west-1b
    
With sudo

    export KUBECONFIG=/etc/kubernetes/admin.conf

Without sudo

    mkdir -p $HOME/.kube 
    sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
    sudo chown $(id -u):$(id -g) $HOME/.kube/config

To delete a node
    
    kubectl cordon <worker-node-ip>
    kubectl drain <worker-node-ip> --ignore-daemonsets --delete-emptydir-data
    kubectl delete node <worker-node-ip>

#### On the worker node 
    Run the "join" command on worker nodes which we got from controller
    kubectl label nodes <worker-node-ip> type=cloud provider=aws service=ec2 region=us-west-1 availability_zone=us-west-1a

## Configure Kafka Cluster
    cd kubernetes/kafka-cluster
    ./create_kafka_cluster.sh
    
To test if kafka is working or not use ./cluster_test.sh

## Configure MongoDB
    cd kubernestes/mongodb
    ./setup_mongo.sh

## Configure KafkaConnect
We use kaniko to build the dockerfile on the go, push it to docker hub and the kafka-connect k8 yaml file downloads it.

_Configure the registry-name in kaniko-pod.yaml and kafka-mongodb-connector.yaml. Update the github repo token as well if using some other repository_
    
    cd kubernetes/kafka-connect
    ./kaniko-deploy.sh

#### Todo
1. Migrate this kafka cluster to EKS
2. Configure network load balancer for the kafka cluster
3. Update aws create_instance script to deploy multiple machine at the same time(_Optional_)
    


    