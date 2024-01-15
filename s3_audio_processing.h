#include <string>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

std::string generate_temp_filename();

void process_s3_audio(Aws::S3::S3Client& client, std::string& bucket_name, std::string& audio_key);

std::string download_audio(
    Aws::S3::S3Client const& client,
    Aws::String const& bucket,
    Aws::String const& key);
