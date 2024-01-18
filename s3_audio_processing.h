#ifndef S3_AUDIO_PROCESSING_H_
#define S3_AUDIO_PROCESSING_H_

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <string>

std::string generate_temp_filename();

void process_s3_audio(Aws::S3::S3Client const& client, std::string const& bucket_name, std::string const& audio_key, std::string const& model);

std::string download_audio(
    Aws::S3::S3Client const& client,
    Aws::String const& bucket,
    Aws::String const& key);

#endif  // S3_AUDIO_PROCESSING_H_
