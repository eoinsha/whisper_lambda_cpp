ARG TARGETPLATFORM
ARG TARGETARCH
FROM public.ecr.aws/sam/build-provided.al2:latest as build

RUN mkdir src
WORKDIR /src
RUN git clone https://github.com/awslabs/aws-lambda-cpp.git
RUN yum update -y && yum install cmake3 make gcc -y
RUN ln -s /usr/bin/cmake3 /usr/bin/cmake
RUN yum install libcurl-devel -y
RUN cd aws-lambda-cpp && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/aws-lambda-cpp && make -j && make install

RUN git clone https://github.com/ggerganov/whisper.cpp.git
RUN cd whisper.cpp && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local && make -j && make install

RUN mkdir -p /app/build
COPY CMakeLists.txt /app/
COPY *.h /app/
COPY *.cpp /app/
WORKDIR /app/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/var/task -DCMAKE_PREFIX_PATH="/aws-lambda-cpp"
RUN make
