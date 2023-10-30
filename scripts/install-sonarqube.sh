#!/bin/bash

container_name='sonarqube'

if [[ "$( docker inspect -f '{{.State.Running}}' ${container_name} )" == "false" ]]; then
    docker ps -a
    docker start $(docker ps -a -q -f status=exited)
    echo "SonarQube is running successfully"

else
    sudo docker run -d --name sonarqube -e SONAR_ES_BOOTSTRAP_CHECKS_DISABLE=true -p 9000:9000 sonarqube:latest
    
fi
