ARG TARGETPLATFORM
ARG TARGETARCH
FROM public.ecr.aws/sam/build-provided.al2023:latest as build

RUN mkdir src
WORKDIR /src
RUN dnf install cmake3 make gcc libcurl-devel openssl-devel git -y

RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp
RUN git clone https://github.com/awslabs/aws-lambda-cpp.git
RUN git clone https://github.com/ggerganov/whisper.cpp.git
RUN git clone https://github.com/Tencent/rapidjson.git

RUN cd aws-lambda-cpp && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local && make -j && make install

RUN cd aws-sdk-cpp && mkdir build && cd build && cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_ONLY="s3"
RUN cd aws-sdk-cpp/build && cmake --build . --config Release && cmake --install . --config Release

RUN cd whisper.cpp && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local && make -j && make install

RUN mv rapidjson/include/rapidjson /usr/local/include

RUN mkdir -p /app/build
COPY CMakeLists.txt /app/
COPY *.h /app/
COPY *.cpp /app/
WORKDIR /app/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/var/task
RUN make -j
