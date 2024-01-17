#ifndef JSON_OUTPUT_H_
#define JSON_OUTPUT_H_

#include <aws/core/utils/json/JsonSerializer.h>
#include <whisper.h>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

using namespace Aws::Utils::Json;

void create_whisper_json_output(whisper_context *ctx, std::string const &outfile)
{
  const int n_segments = whisper_full_n_segments(ctx);

  Aws::Utils::Array<JsonValue> segments(n_segments);
  for (int i = 0; i < n_segments; ++i)
  {
    JsonValue segment;

    const char *text = whisper_full_get_segment_text(ctx, i);
    const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
    const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

    segment.WithInteger("id", i);
    segment.WithDouble("start", t0 / 100.0);
    segment.WithDouble("end", t1 / 100.0);
    segment.WithString("text", text);

    segments[i] = std::move(segment);
  }

  JsonValue transcript;
  transcript.WithArray("segments", segments);
  JsonValue doc;
  doc.WithObject("transcript", transcript);

  std::ofstream ofile(outfile);
  ofile << doc.View().WriteReadable();
  ofile.close();

  std::cout << doc.View().WriteReadable() << std::endl;
}

#endif  // JSON_OUTPUT_H_
