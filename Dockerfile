FROM public.ecr.aws/sam/build-provided.al2:latest as build

# RUN CMAKE_ARGS="-DLLAMA_AVX2=ON -DLLAMA_BLAS=ON -DLLAMA_BLAS_VENDOR=OpenBLAS" pip install llama-cpp-python
RUN mkdir src
WORKDIR /src
RUN git clone https://github.com/awslabs/aws-lambda-cpp.git
RUN yum update -y && yum install cmake3 make gcc -y
RUN ln -s /usr/bin/cmake3 /usr/bin/cmake
RUN yum install libcurl-devel -y
RUN cd aws-lambda-cpp && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/aws-lambda-cpp && make -j && make install

RUN mkdir -p /app/build
COPY CMakeLists.txt /app/
COPY *.cpp /app/
WORKDIR /app/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/var/task -DCMAKE_PREFIX_PATH=/aws-lambda-cpp
RUN make
