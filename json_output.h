#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <whisper.h>

void create_whisper_json_output(whisper_context *ctx, std::string const& outfile) {
    const int n_segments = whisper_full_n_segments(ctx);

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    rapidjson::Value segments(rapidjson::kArrayType);

    for (int i = 0; i < n_segments; ++i) {
        rapidjson::Value segment(rapidjson::kObjectType);

        const char* text = whisper_full_get_segment_text(ctx, i);
        const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
        const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

        segment.AddMember("id", i, allocator);
        segment.AddMember("start", t0, allocator);
        segment.AddMember("end", t1, allocator);
        segment.AddMember("text", rapidjson::StringRef(text), allocator);

        segments.PushBack(segment, allocator);
    }

    doc.AddMember("segments", segments, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream ofile(outfile);
    ofile << buffer.GetString();
    ofile.close();

    std::cout << buffer.GetString() << std::endl;
}