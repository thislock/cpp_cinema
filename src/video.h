
#ifndef CINEMA_VIDEO_LOADER_H
#define CINEMA_VIDEO_LOADER_H

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>
  #include <libavutil/imgutils.h>
}

struct Frame {
  AVFrame* frame     = nullptr;
  AVFrame* rgb_frame = nullptr;
  AVPacket* packet   = nullptr;
  Frame();
  ~Frame();
};

class Video {

public:
  Video(std::string& input_file);
  ~Video();

private:
  AVFormatContext* format_context = nullptr;
  AVCodecParameters* codec_parameters = nullptr;
  SwsContext* sws_context = nullptr;
  const AVCodec* codec = nullptr;
  AVCodecContext * codec_context = nullptr;
  uint8_t* rgb_buffer = nullptr;

  Frame * frame = nullptr;

  int video_stream_index = 0;

  int Init(std::string & input_file);

  int open_video_file(std::string & video_file);
  int find_video_stream();

};


#endif