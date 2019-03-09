FROM ubuntu:18.04
RUN mkdir -p /service/knapsack
COPY  ./api.tar /service/knapsack
WORKDIR /service/knapsack
RUN tar -xf api.tar
RUN cp knapLibs.conf /etc/ld.so.conf.d/
RUN ldconfig
CMD ./maerskApi
