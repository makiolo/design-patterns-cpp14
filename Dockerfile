ARG IMAGE
FROM $IMAGE

ARG MODE
ARG COMPILER
ARG COMPILER_VERSION
ARG COMPILER_LIBCXX
ARG CC
ARG CXX
ARG PACKAGE
ARG CONAN_TOKEN

ENV MODE=${MODE}
ENV COMPILER=${COMPILER}
ENV COMPILER_VERSION=${COMPILER_VERSION}
ENV COMPILER_LIBCXX=${COMPILER_LIBCXX}
ENV CC=${CC}
ENV CXX=${CXX}
ENV PACKAGE=${PACKAGE}
ENV CONAN_TOKEN=${CONAN_TOKEN}

WORKDIR /code
ADD requirements.txt /code/
RUN pip install -r requirements.txt

RUN curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -
RUN sudo apt-get install -y nodejs

RUN conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"
RUN conan remote add npm-mas-mas "https://api.bintray.com/conan/npm-mas-mas/testing"
RUN conan user -p $CONAN_TOKEN -r npm-mas-mas makiolo

COPY . /code
RUN bash -c /code/entrypoint.sh

