#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>

#include <aws/lambda-runtime/runtime.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/core/platform/Environment.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "s3_audio_processing.h"
#include "transcription.h"
#include "common.h"

using namespace aws::lambda_runtime;

char const TAG[] = "WHISPER_HANDLER";

static invocation_response whisper_handler(invocation_request const &req, Aws::S3::S3Client &client)
{
  using namespace Aws::Utils::Json;
  AWS_LOGSTREAM_INFO(TAG, "Invoked with payload: " << req.payload);

  JsonValue json(req.payload);
  if (!json.WasParseSuccessful()) {
      return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
  }

  auto v = json.View();

  if (!v.ValueExists("s3bucket") || !v.ValueExists("s3key") || !v.GetObject("s3bucket").IsString() ||
      !v.GetObject("s3key").IsString()) {
      return invocation_response::failure("Missing input value s3bucket or s3key", "InvalidJSON");
  }

  auto bucket = v.GetString("s3bucket");
  auto key = v.GetString("s3key");
  try {
    process_s3_audio(client, bucket, key);
    return invocation_response::success("transcription done", "application/json");
  } catch (const std::runtime_error &e) {
    return invocation_response::failure(e.what(), "RuntimeError");
  }
}
