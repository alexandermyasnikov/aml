FROM gcc

RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y cmake make
RUN DEBIAN_FRONTEND=noninteractive apt install -y g++10
RUN DEBIAN_FRONTEND=noninteractive apt install -y libboost-program-options-dev

RUN git clone https://github.com/fmtlib/fmt.git \
  && cd fmt && mkdir build && cd build \
  && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. && make -j \
  && make install

RUN git clone https://github.com/gabime/spdlog.git \
  && cd spdlog && mkdir build && cd build \
  && cmake .. && make -j \
  && make install

RUN git clone https://github.com/catchorg/Catch2.git \
  && cd Catch2 && mkdir build && cd build \
  && cmake .. && make -j \
  && make install

COPY . /aml
RUN rm -rf /aml/build
RUN cd /aml && cmake -S. -B./build && cmake --build ./build

CMD cd /aml && ./build/aml/aml_tests
