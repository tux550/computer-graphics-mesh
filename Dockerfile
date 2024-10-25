# Ubuntu 22.04
FROM ubuntu:22.04
# Update
RUN apt update
# Install g++ and make
RUN apt install -y g++ make
# Install epstopdf
RUN apt install -y texlive-font-utils
# Setup working directory
RUN mkdir /app
WORKDIR /app
RUN mkdir /app/outputs