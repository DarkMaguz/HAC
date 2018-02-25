# Base the image off of Debian.
FROM debian:stretch

# Getting the prerequisite packages.
RUN apt-get -y update && apt-get -y install nano build-essential autoconf automake libtool libmariadbclient-dev libmysql++-dev libmysql++3v5 libmysqlcppconn-dev --no-install-recommends

# Make build directory.
RUN mkdir -p /tmp/app

# Copy source code to temporary dir.
ADD app/ /tmp/app/

COPY build-app.sh /tmp/app

RUN chmod 755 /tmp/app/*.sh

# Build the app.
RUN /tmp/app/build-app.sh

# Make the base and log directory for the app.
RUN mkdir -p /usr/app/log

# Expose port.
EXPOSE 4337

# Set our working directory.
WORKDIR /usr/app

# Set default command.
CMD ["hacd"]

