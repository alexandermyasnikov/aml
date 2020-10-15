FROM gcc AS aml_builder

ARG JOBS=1

RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y cmake make
RUN DEBIAN_FRONTEND=noninteractive apt install -y libboost-program-options-dev
RUN DEBIAN_FRONTEND=noninteractive apt install -y rpm

RUN git clone https://github.com/fmtlib/fmt.git \
  && cd fmt \
  && cmake -S. -B./build -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
  && cmake --build ./build -j $JOBS \
  && cmake --build ./build --target install

RUN git clone https://github.com/gabime/spdlog.git \
  && cd spdlog \
  && cmake -S. -B./build \
  && cmake --build ./build -j $JOBS \
  && cmake --build ./build --target install

RUN git clone -b v2.x https://github.com/catchorg/Catch2.git \
  && cd Catch2 \
  && cmake -S. -B./build \
  && cmake --build ./build -j $JOBS \
  && cmake --build ./build --target install

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

