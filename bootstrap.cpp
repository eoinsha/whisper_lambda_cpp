#include <iostream>

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogLevel.h>

#include <aws/lambda-runtime/runtime.h>
#include "handler.h"

using namespace std;
using namespace aws::lambda_runtime;
using namespace Aws::Utils::Logging;

int main()
{
  cout << "Running Lambda Handler " << __DATE__ << "," << __TIME__ << endl;
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  options.loggingOptions.logger_create_fn = [] { return std::make_shared<ConsoleLogSystem>(LogLevel::Info); };
  options.loggingOptions.crt_logger_create_fn = []()
  {
    return Aws::MakeShared<Aws::Utils::Logging::DefaultCRTLogSystem>(
        "CRTLogSystem", Aws::Utils::Logging::LogLevel::Warn);
  };
  Aws::InitAPI(options);

  {
    Aws::Client::ClientConfiguration config;
    config.region = Aws::Environment::GetEnv("AWS_REGION");

  Aws::S3::S3Client client(config);
    auto handler_fn = [&client](aws::lambda_runtime::invocation_request const &req) {
      return whisper_handler(req, client);
    };
    run_handler(handler_fn);
  }

  Aws::ShutdownAPI(options);
  return 0;
}