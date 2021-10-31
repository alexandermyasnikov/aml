FROM gcc:11.2 AS aml_builder

ARG JOBS=1

RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y cmake make
RUN DEBIAN_FRONTEND=noninteractive apt install -y libboost-program-options-dev
RUN DEBIAN_FRONTEND=noninteractive apt install -y rpm
RUN DEBIAN_FRONTEND=noninteractive apt install -y catch2 libfmt-dev libspdlog-dev

COPY . /aml

WORKDIR /aml

RUN rm -rf ./build
RUN cmake -S. -B./build \
    -D CMAKE_INSTALL_LIBDIR=lib \
    -D CMAKE_INSTALL_INCLUDEDIR=include \
    -D AML_PACKAGE_TYPE=DEB \
  && cmake --build ./build -j $JOBS \
  && cmake --build ./build --target package -j $JOBS

ENTRYPOINT ["./build/amlc"]



FROM ubuntu:latest as aml_compact
COPY --from=aml_builder /aml /aml_tmp
RUN find /aml_tmp -maxdepth 2 -name *.deb
RUN apt install `find /aml_tmp -maxdepth 2 -name *.deb`

RUN rm -rf aml_tmp
ENTRYPOINT ["amlc"]

