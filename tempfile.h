#include <filesystem>
#include <string>
#include <sstream>
#include <random>

/**
 * Create a temporary filename with the given extension. The file is not created.
 */
std::string generate_temp_filename(std::string const &ext)
{
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<unsigned long long> distribution;

  std::stringstream filename;
  filename << std::filesystem::temp_directory_path().string() << "/tempfile_" << distribution(generator) << "." << ext;

  return filename.str();
}

class FileCleaner
{
public:
  explicit FileCleaner(const std::string &filename) : m_filename(filename) {}
  ~FileCleaner()
  {
    std::filesystem::remove(m_filename);
  }

private:
  std::string m_filename;
};
