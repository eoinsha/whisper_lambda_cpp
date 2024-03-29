#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>

#define COMMON_SAMPLE_RATE 16000

bool read_wav(
  const std::string & fname,
  std::vector<float> * pcmf32,
  std::vector<std::vector<float>> * pcmf32s,
  bool stereo
);
#endif  // COMMON_H_
