#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "s3_audio_processing.h"
#include "transcription.h"

char const TAG[] = "S3_AUDIO_PROCESSING";


class FileCleaner {
public:
    explicit FileCleaner(const std::string& filename) : m_filename(filename) {}
    ~FileCleaner() {
        std::filesystem::remove(m_filename);
    }

private:
    std::string m_filename;
};

void process_s3_audio(Aws::S3::S3Client& client, std::string& bucket_name, std::string& audio_key) {
  auto audio_filename = download_audio(client, bucket_name, audio_key);
  FileCleaner cleaner(audio_filename); // Auto-deletes the file when cleaner goes out of scope
  transcribe(audio_filename);
}

std::string download_audio(
    Aws::S3::S3Client const& client,
    Aws::String const& bucket,
    Aws::String const& key)
{
    using namespace Aws;

    AWS_LOGSTREAM_INFO(TAG, "Attempting to download file from s3://" << bucket << "/" << key);

    S3::Model::GetObjectRequest request;
    request.WithBucket(bucket).WithKey(key);

    auto outcome = client.GetObject(request);
    if (outcome.IsSuccess()) {
        AWS_LOGSTREAM_INFO(TAG, "Download completed!");
        auto& s3_stream = outcome.GetResult().GetBody();
        std::string filename = generate_temp_filename();
        std::cout << filename << std::endl;
        std::fstream audio_file(filename, std::ios::out | std::ios::binary);
        audio_file << s3_stream.rdbuf();
        std::cout << "Wrote " << filename << std::endl;
        return filename;
    }
    else {
        AWS_LOGSTREAM_ERROR(TAG, "Failed with error: " << outcome.GetError());
        throw std::runtime_error("Failed to read audio file: " + outcome.GetError().GetMessage());
    }
}

std::string generate_temp_filename() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<unsigned long long> distribution;

    std::stringstream filename;
    filename << std::filesystem::temp_directory_path().string() << "/tempfile_" << distribution(generator) << ".wav";

    return filename.str();
}