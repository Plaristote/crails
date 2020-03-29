FROM debian:buster

ENV LD_LIBRARY_PATH /usr/local/lib

RUN apt-get -y --allow-unauthenticated update && \
    apt-get -y --allow-unauthenticated upgrade && \
    apt-get -y install wget \
  cmake \
  build-essential \
  libbz2-dev \
  libssl-dev \
  git \
  ruby \
  ruby-bundler \
  ruby-dev \
  nodejs

# Downgrade to gcc-7 to solve compatibility issues with odb 2.5
RUN apt-get -y install gcc-7 g++-7 gcc-7-plugin-dev
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 20

WORKDIR /tmp/compile-dir

# BEGIN Crails dependencies
COPY boost.sh boost.sh
RUN bash /tmp/compile-dir/boost.sh

COPY cppnetlib.sh cppnetlib.sh
RUN bash ./cppnetlib.sh
# END Crails dependencies

COPY crails.sh crails.sh
RUN bash ./crails.sh
ENV CRAILS_SHARED_DIR /usr/local/share/crails

# BEGIN Dependencies
# END Dependencies

RUN mkdir -p /opt/webapp
WORKDIR /opt/webapp
ENV CRAILS_RUBY_BUNDLE_PATH /opt/webapp/docker
COPY package.sh /usr/local/bin/package.sh
RUN chmod +x /usr/local/bin/package.sh
