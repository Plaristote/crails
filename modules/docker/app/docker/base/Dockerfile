FROM debian:jessie

RUN apt-get -y update && apt-get -y install wget \
 cmake \
 build-essential \
 libbz2-dev \
 libssl-dev \
 git \
 ruby \
 ruby-dev \
 nodejs \
 libpq-dev

RUN gem install bundler

WORKDIR /tmp/compile-dir

COPY compile-boost.sh compile-boost.sh
RUN bash /tmp/compile-dir/compile-boost.sh

COPY compile-cppnetlib.sh compile-cppnetlib.sh
RUN bash ./compile-cppnetlib.sh

RUN apt-get -y install scons
COPY compile-mongodb-cxx-driver.sh compile-mongodb-cxx-driver.sh
RUN bash ./compile-mongodb-cxx-driver.sh

COPY compile-odb.sh compile-odb.sh
RUN bash ./compile-odb.sh

COPY compile-libmemcached.sh compile-libmemcached.sh
RUN bash ./compile-libmemcached.sh

COPY compile-magick++.sh compile-magick++.sh
RUN bash ./compile-magick++.sh

COPY compile-crails.sh compile-crails.sh
RUN bash ./compile-crails.sh
ENV CRAILS_SHARED_DIR /usr/local/share/crails

WORKDIR /
RUN rm -Rf /tmp/compile-dir

RUN useradd -ms /bin/bash crails
VOLUME ["/home/crails"]

COPY make-application-package.sh /usr/local/bin/make-application-package.sh
RUN chmod +x /usr/local/bin/make-application-package.sh
