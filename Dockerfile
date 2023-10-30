FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y linux-modules-extra-$(uname -r) && apt-get -y install can-utils
RUN apt-get install -y libbluetooth-dev && apt-get install -y libsqlite3-dev
RUN apt install -y iproute2 && apt install -y build-essential && apt-get -y install manpages-dev
RUN apt install -y make
RUN apt-get -y update
#RUN modprobe vcan
COPY . /root
WORKDIR /root/Telematics-Rpi/
RUN . /root  gcc /src/can_bus/virtual_can_server.c -o virtualcan -lpthread
RUN  . /root make main
