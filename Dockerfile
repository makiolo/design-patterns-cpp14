ARG IMAGE
FROM $IMAGE

RUN curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -
RUN sudo apt-get install -y nodejs

ARG MODE
ARG COMPILER
ARG COMPILER_VERSION
ARG COMPILER_LIBCXX
ARG CC
ARG CXX
ARG CONAN_TOKEN
ARG NPP_CI=FALSE

ENV MODE=${MODE}
ENV COMPILER=${COMPILER}
ENV COMPILER_VERSION=${COMPILER_VERSION}
ENV COMPILER_LIBCXX=${COMPILER_LIBCXX}
ENV CC=${CC}
ENV CXX=${CXX}
ENV CONAN_TOKEN=${CONAN_TOKEN}
ENV NPP_CI=${NPP_CI}

RUN mkdir -p /tmp
WORKDIR /tmp
COPY requirements.txt /tmp/
RUN pip install -r requirements.txt

RUN conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"
RUN conan remote add npm-mas-mas "https://api.bintray.com/conan/npm-mas-mas/testing"
RUN conan user -p $CONAN_TOKEN -r npm-mas-mas makiolo

COPY --chown=1000:1001 . /tmp
cmd /tmp/entrypoint.sh

