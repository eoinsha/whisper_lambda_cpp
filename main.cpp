#include <aws/lambda-runtime/runtime.h>
#include <whisper.h>
#include <iostream>
#include <fstream>
#include <thread>

using namespace std;
using namespace aws::lambda_runtime;

int n_threads = std::thread::hardware_concurrency();

static invocation_response my_handler(invocation_request const& req)
{
  struct whisper_context_params cparams;
  cparams.use_gpu = false;
  struct whisper_context * ctx = whisper_init_from_file_with_params("./ggml-model-whisper-tiny.en.bin", cparams);

  if (ctx == nullptr) {
    return invocation_response::failure("error: failed to initialise whisper context", "error_type");
  }
  std::vector<float> pcmf32;               // mono-channel F32 PCM
  std::vector<std::vector<float>> pcmf32s; // stereo-channel F32 PCM

  ifstream input_file("wasm-test-16.wav", std::ios::binary);
  input_file.seekg(0, std::ios::end);
  int num_samples = input_file.tellg() / sizeof(float);
  input_file.seekg(0, std::ios::beg);

  pcmf32.resize(num_samples);
  for (int i = 0; i < num_samples; i++)
  {
    input_file.read(reinterpret_cast<char *>(&pcmf32[i]), sizeof(float));
  }

  pcmf32s.push_back(pcmf32); // make stereo by duplicating mono channel
  pcmf32s.push_back(pcmf32);

  whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
  wparams.strategy = WHISPER_SAMPLING_GREEDY;
  wparams.print_realtime   = false;
  // wparams.print_progress   = true;
  // wparams.print_timestamps = true;
  wparams.print_special    = false;
  wparams.translate        = false;
  wparams.language         = "en";
  wparams.detect_language  = false;
  wparams.n_threads        = n_threads;
  wparams.n_max_text_ctx   = -1;
  wparams.offset_ms        = 0;
  wparams.duration_ms      = 0;

  wparams.token_timestamps = true;
  wparams.thold_pt         = 0.01f;
  wparams.max_len          = 60;
  wparams.split_on_word    = false;

  wparams.speed_up         = false;
  wparams.debug_mode       = false;

  wparams.tdrz_enable      = false; // TODO - try later

  wparams.initial_prompt   = 0;

  if (whisper_full_parallel(ctx, wparams, pcmf32.data(), pcmf32.size(), 1) != 0) {
    return invocation_response::failure("error: failed to process audio", "ProcessAudioFail");
  }
  const int n_segments = whisper_full_n_segments(ctx);
  for (int i = 0; i < n_segments; ++i)
  {
    const char *text = whisper_full_get_segment_text(ctx, i);

    const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
    const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

    cout << "t0: " << t0 << " t1: " << t1 << " text: " << text << endl;
  }
  // wparams.greedy.best_of   = params.best_of;
  // wparams.beam_search.beam_size = params.beam_size;

  // wparams.temperature_inc  = params.no_fallback ? 0.0f : wparams.temperature_inc;
  // wparams.entropy_thold    = params.entropy_thold;
  // wparams.logprob_thold    = params.logprob_thold;

  return invocation_response::success("transcription done" /*payload*/,
                    "application/json" /*MIME type*/);
}


int main()
{
  run_handler(my_handler);
  return 0;
}