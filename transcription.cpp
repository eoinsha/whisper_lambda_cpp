#include "transcription.h"

#include <curl/curl.h>
#include <whisper.h>

#include <filesystem>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <stdexcept>

#include "json_output.h"
#include "common.h"

int n_threads = std::thread::hardware_concurrency();
std::filesystem::path model_dir = std::filesystem::temp_directory_path() / "models";


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool download_file(const std::string& url, const std::string& file_path) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(file_path.c_str(), "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        return true;
    }

    return false;
}

std::string download_model_if_needed(const std::string& model) {
    std::string base_url = "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-";
    std::string model_path = model_dir / (model + ".bin");
    std::string url = base_url + model + ".bin";

    // Check if the model already exists
    if (!std::filesystem::exists(model_path)) {
      std::cout << "Downloading model: " << model << " from " << url << std::endl;
      std::filesystem::create_directories(model_dir);

        if (!download_file(url, model_path)) {
            std::cerr << "Failed to download the model" << std::endl;
        }
    } else {
        std::cout << "Model already exists: " << model_path << std::endl;
    }
    return model_path;
}

void transcribe(std::string const& fname_inp, std::string const& model, std::string const& outfile) {
  struct whisper_context_params cparams;
  cparams.use_gpu = false;
  std::cout << "Transcribing " << fname_inp << std::endl;
  const char* system_info = whisper_print_system_info();
  std::cout << "system_info " << system_info << std::endl;

  const std::string model_path = download_model_if_needed(model);
  struct whisper_context * ctx = whisper_init_from_file_with_params(model_path.data(), cparams);

  if (ctx == nullptr) {
    throw std::runtime_error("Error: Failed to initialise whisper context");
  }

  std::vector<float> pcmf32;                // mono-channel F32 PCM
  std::vector<std::vector<float>> pcmf32s;  // stereo-channel F32 PCM

  bool diarize = false;

  std::cout << "Reading wav " << fname_inp << std::endl;
  if (!::read_wav(fname_inp, &pcmf32, &pcmf32s, diarize)) {
      fprintf(stderr, "error: failed to read WAV file '%s'\n", fname_inp.c_str());
    throw std::runtime_error("Error: Failed to read WAV file '" + fname_inp + "'");
  }

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

  wparams.tdrz_enable      = false;  // TODO(eoin) - try later

  wparams.initial_prompt   = 0;

  if (whisper_full_parallel(ctx, wparams, pcmf32.data(), pcmf32.size(), 1) != 0) {
    throw std::runtime_error("Error: Failed to process audio");
  }

  // const int n_segments = whisper_full_n_segments(ctx);
  // for (int i = 0; i < n_segments; ++i)
  // {
  //   const char *text = whisper_full_get_segment_text(ctx, i);

  //   const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
  //   const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

  //   std::cout << "t0: " << t0 << " t1: " << t1 << " text: " << text << std::endl;
  // }
  create_whisper_json_output(ctx, outfile);
  // wparams.greedy.best_of   = params.best_of;
  // wparams.beam_search.beam_size = params.beam_size;

  // wparams.temperature_inc  = params.no_fallback ? 0.0f : wparams.temperature_inc;
  // wparams.entropy_thold    = params.entropy_thold;
  // wparams.logprob_thold    = params.logprob_thold;

  whisper_print_timings(ctx);
  whisper_free(ctx);
}
