FROM gcc

RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y cmake make
RUN DEBIAN_FRONTEND=noninteractive apt install -y libboost-program-options-dev

RUN git clone https://github.com/fmtlib/fmt.git \
  && cd fmt \
  && cmake -S. -B./build -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
  && cmake --build ./build \
  && cmake --build ./build --target install

RUN git clone https://github.com/gabime/spdlog.git \
  && cd spdlog \
  && cmake -S. -B./build \
  && cmake --build ./build \
  && cmake --build ./build --target install

RUN git clone https://github.com/catchorg/Catch2.git \
  && cd Catch2 \
  && cmake -S. -B./build \
  && cmake --build ./build \
  && cmake --build ./build --target install

COPY . /aml

WORKDIR /aml

RUN rm -rf ./build
RUN cmake -S. -B./build \
  && cmake --build ./build

ENTRYPOINT ["./build/aml"]
