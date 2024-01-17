# Whisper C++ in Lambda 🎤 🚀

This repository contains the source code and AWS SAM template for deploying and running OpenAI's Whisper TTS (Text-to-Speech) model in an AWS Lambda environment using a C++ custom runtime. It uses [whisper.cpp](https://github.com/ggerganov/whisper.cpp) to remove the need for large dependencies.

## Overview

This SAM project deploys and S3 Bucket and Arm and X86-64 versions of a Lambda function that process WAV audio placed in that bucket automatically. A transcript JSON is placed in the same S3 bucket using the same key as the input but with a `.json` extension.

## Repository Structure

- `template.yaml`: AWS SAM template defining the Lambda functions and related AWS resources.
- `bootstrap.cpp`: The main entry point for the Lambda function.
- `handler.h`: Header file for the Lambda function handler, managing Whisper TTS processing.
- `s3_audio_processing.cpp`: Handles audio file processing and S3 interactions.
- `test_main.cpp`: Main file for testing the setup locally.
- `CMakeLists.txt`: CMake configuration for building the project.

## Prerequisites

- AWS SAM CLI
- Docker
- (Optionally) A C++ build environment, if you want to build and run locally.

## Deploying with AWS SAM

```sh
sam build
sam deploy --guided
```

## Usage

After deployment, the Lambda function can be triggered via S3 events. 

```sh
export BUCKET_NAME=whisper-lambda-audio-<REGION>-<ACCOUNT_ID>
aws s3 cp test-16.wav s3://${BUCKET_NAME}
```

You can monitor CloudWatch logs and the S3 Bucket for transcript and/or error output.

By default, the `tiny.en` model is used. To switch, you can add `model` property to the Lambda's input JSON and invoke it manually.

```json
{
    "s3bucket": "whisper-lambda-audio-eu-west-1-123456789012",
    "s3key": "episode-108.wav",
    "model": "small.en"
}
```

To permanently change the default model, update the `InputTemplate` in [`template.yaml`](./template.yaml) or the fallback value in [`handler.h`](./handler.h).

## Local Build and Testing

To build locally, first install:
- [aws-sdk-cpp](https://github.com/aws/aws-sdk-cpp)
- [aws-lambda-cpp](https://github.com/awslabs/aws-lambda-cpp)
- [whisper.cpp](https://github.com/ggerganov/whisper.cpp/)

Then, build the project with:
```sh
mkdir build
cd build/
cmake ..
make -j
```

You can then run the test binary as follows.
```sh
./build/whisper_lambda_cpp_test ${BUCKET_NAME} audio.wav
```

## License

MIT [LICENSE](./LICENSE)
