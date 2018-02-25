#!/bin/bash

: "${MYSQL_USER:?MYSQL_USER environment variable must be set!}"
: "${MYSQL_PASSWORD:?MYSQL_PASSWORD environment variable must be set!}"
: "${GMAIL_USER:?GMAIL_USER environment variable must be set!}"
: "${GMAIL_PASSWORD:?GMAIL_PASSWORD environment variable must be set!}"

# Cleanup old containers.
docker rm -f hacdb myadmin hac-server smtp

# Build Dockerfile.
docker build -t darkmagus/hac-server .

#DATA=-v $(pwd)/sql:/docker-entrypoint-initdb.d
#-v $(pwd)/datadir:/var/lib/mysql

# Launch services.
docker run --name hacdb -d -p 3306:3306 -e MYSQL_ROOT_PASSWORD=root -e MYSQL_DATABASE=hac01 -e MYSQL_USER=${MYSQL_USER} -e MYSQL_PASSWORD=${MYSQL_PASSWORD} -v $(pwd)/sql:/docker-entrypoint-initdb.d mysql/mysql-server:5.7
docker run --name myadmin -d -p 8081:80 -e MYSQL_ROOT_PASSWORD=root --link hacdb:db phpmyadmin/phpmyadmin:4.7
docker run --name smtp -d -p 25:25 -e RELAY_NETWORKS=:172.17.0.0/24 -e GMAIL_USER=${GMAIL_USER} -e GMAIL_PASSWORD=${GMAIL_PASSWORD} -e DISABLE_IPV6=true namshi/smtp:latest

# Run the server.
docker run -it --name hac-server -v $(pwd)/log:/user/app/log --link hacdb:hacdb --link smtp:smtp -p 4337:4337 darkmagus/hac-server

