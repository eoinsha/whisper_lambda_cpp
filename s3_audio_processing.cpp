#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include "tempfile.h"
#include "s3_audio_processing.h"
#include "transcription.h"

char const TAG[] = "S3_AUDIO_PROCESSING";

void upload_file(Aws::S3::S3Client &client, std::string const &source_file, std::string &bucket_name, std::string &target_key)
{
  using namespace Aws;
  AWS_LOGSTREAM_INFO(TAG, "Attempting to upload file from " << source_file << " to s3://" << bucket_name << "/" << target_key);

  S3::Model::PutObjectRequest request;
  request.WithBucket(bucket_name).WithKey(target_key);

  std::shared_ptr<Aws::IOStream> inputData = Aws::MakeShared<Aws::FStream>("UploadAllocationTag",
                                                                           source_file.c_str(),
                                                                           std::ios_base::in | std::ios_base::binary);
  request.SetBody(inputData);

  auto outcome = client.PutObject(request);
  if (!outcome.IsSuccess())
  {
    AWS_LOGSTREAM_ERROR(TAG, "Failed with error: " << outcome.GetError());
    throw std::runtime_error("Failed to upload file: " + outcome.GetError().GetMessage());
  }
  else
  {
    AWS_LOGSTREAM_INFO(TAG, "Upload completed");
  }
}

void process_s3_audio(Aws::S3::S3Client &client, std::string &bucket_name, std::string &audio_key, std::string &model)
{
  auto audio_filename = download_audio(client, bucket_name, audio_key);
  FileCleaner cleaner(audio_filename); // Auto-deletes the file when cleaner goes out of scope
  std::string const outfile = generate_temp_filename(".json");
  transcribe(audio_filename, model, outfile);
  std::string transcript_key = audio_key + ".transcript.json";
  upload_file(client, outfile, bucket_name, transcript_key);
}

std::string download_audio(
    Aws::S3::S3Client const &client,
    Aws::String const &bucket,
    Aws::String const &key)
{
  using namespace Aws;

  AWS_LOGSTREAM_INFO(TAG, "Attempting to download file from s3://" << bucket << "/" << key);

  S3::Model::GetObjectRequest request;
  request.WithBucket(bucket).WithKey(key);

  auto outcome = client.GetObject(request);
  if (outcome.IsSuccess())
  {
    AWS_LOGSTREAM_INFO(TAG, "Download completed");
    auto &s3_stream = outcome.GetResult().GetBody();
    std::string filename = generate_temp_filename(".wav");
    std::cout << filename << std::endl;
    std::fstream audio_file(filename, std::ios::out | std::ios::binary);
    audio_file << s3_stream.rdbuf();
    std::cout << "Wrote " << filename << std::endl;
    return filename;
  }
  else
  {
    AWS_LOGSTREAM_ERROR(TAG, "Failed with error: " << outcome.GetError());
    throw std::runtime_error("Failed to read audio file: " + outcome.GetError().GetMessage());
  }
}