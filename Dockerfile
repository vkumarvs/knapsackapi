FROM ubuntu:18.04
RUN mkdir -p /service/knapsack
WORKDIR /service/knapsack
COPY  ./api.tar /service/knapsack
RUN tar -xf api.tar
COPY  knapLibs.conf /etc/ld.so.conf.d/
RUN ldconfig
CMD ./maerskApi
