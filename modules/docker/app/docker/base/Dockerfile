FROM debian:buster

ENV LD_LIBRARY_PATH /usr/local/lib

RUN apt-get -y update && apt-get -y install wget \
  cmake \
  build-essential \
  libbz2-dev \
  libssl-dev \
  git \
  ruby \
  ruby-bundler \
  ruby-dev \
  nodejs \
  libpq-dev

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

WORKDIR /
RUN rm -Rf /tmp/compile-dir

RUN useradd -ms /bin/bash crails
VOLUME ["/home/crails"]

COPY make-application-package.sh /usr/local/bin/make-application-package.sh
RUN chmod +x /usr/local/bin/make-application-package.sh
