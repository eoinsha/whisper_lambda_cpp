#include <iostream>
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogLevel.h>

#include "s3_audio_processing.h"

using namespace std;
using namespace Aws::Utils::Logging;

int main(int argc, char **argv)
{
  std::string bucket = argv[1];
  std::string key = argv[2];

  cout << "Running text execution " << __DATE__ << "," << __TIME__ << endl;
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  options.loggingOptions.logger_create_fn = [] { return std::make_shared<ConsoleLogSystem>(LogLevel::Info); };
  options.loggingOptions.crt_logger_create_fn = []()
  {
    return Aws::MakeShared<Aws::Utils::Logging::DefaultCRTLogSystem>(
        "CRTLogSystem", Aws::Utils::Logging::LogLevel::Warn);
  };
  Aws::InitAPI(options);

  std::string filename = "wasm-test-16.wav";
  Aws::Client::ClientConfiguration config;
  config.region = "eu-west-1";
  Aws::S3::S3Client client(config);

  try {
    process_s3_audio(client, bucket, key);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }

  cout << "Shutting down" << endl;
  Aws::ShutdownAPI(options);
  return 0;
}