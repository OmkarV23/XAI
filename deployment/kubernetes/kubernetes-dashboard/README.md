# Access Kubernetes Dashboard
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /tmp/nginx.key -out /tmp/nginx.crt -subj "/CN=nginxsvc/O=nginxsvc"
    cp /tmp/nginx.crt OME/certs/tls.crt
    cp /tmp/nginx.key OME/certs/tls.key

### Download dashboard config file

    wget https://raw.githubusercontent.com/kubernetes/dashboard/v2.7.0/aio/deploy/recommended.yaml
    kubectl create namespace kubernetes-dashboard

### Create dashboard secret
    kubectl create secret generic kubernetes-dashboard-certs --from-file=$HOME/certs -n kubernetes-dashboard
    kubectl describe secret kubernetes-dashboard-certs -n kubernetes-dashboard

### Apply configs
    kubectl apply -f recommended.yaml
    kubectl apply -f service-acc.yaml
    kubectl apply -f cluster-role-binding.yaml

### Check service status
    kubectl get pod -n kubernetes-dashboard
    kubectl get service -n kubernetes-dashboard  

### Check UI status
    curl -k http://<public-ip>:8002/api/v1/namespaces/kubernetes-dashboard/services/https:kubernetes-dashboard:/proxy/
    curl -k https://<public-ip>:32000

### Generate token to login
    kubectl -n kubernetes-dashboard create token admin-user