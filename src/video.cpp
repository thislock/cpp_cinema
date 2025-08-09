
#include <iostream>

#include "video.h"

#include <iostream>
#include <memory>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
  #include <direct.h>
  #define getcwd _getcwd
  #define PATH_MAX _MAX_PATH
#else
  #include <unistd.h>
  #include <limits.h>
#endif

Frame::Frame() {
  this->frame     = av_frame_alloc();
  this->rgb_frame = av_frame_alloc();
  this->packet    = av_packet_alloc();
}

Frame::~Frame() {
  av_frame_free(&this->frame);
  av_frame_free(&this->rgb_frame);
  av_packet_free(&this->packet);
}

std::unique_ptr<char[]> get_current_directory() {
  auto buffer = std::make_unique<char[]>(PATH_MAX);
  if (buffer == NULL) {
    perror("malloc");
    return nullptr;
  }

  if (getcwd(buffer.get(), PATH_MAX) != nullptr) {
    return buffer;
  } else {
    perror("getcwd");
    return nullptr;
  }
}


Video::Video(std::string& input_file) {
  int result = this->Init(input_file);
}

#include "error.h"

int Video::open_video_file(std::string & file) {
  // open the video file
  if (avformat_open_input(&this->format_context, file.c_str(), nullptr, nullptr) != 0) {
    // couldn't find file "something.mp4"
    std::string error_msg = "couldn't find file, \"";
    error_msg.append(file);
    error_msg.append("\"");
    showErrorPopup(error_msg);
    return -1;
  }

  if (avformat_find_stream_info(format_context, nullptr) < 0) {
    showErrorPopup("couldn't find stream info.");
    return -1;
  }

  int result = this->find_video_stream();
  if (result != 0) {
    return result;
  }

  return 0;
}

int Video::find_video_stream() {
  for (unsigned int i = 0; i < format_context->nb_streams; i++) {
    if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      this->video_stream_index = i;
      break;
    }
  }
  if (this->video_stream_index == -1) {
    showErrorPopup("couldn't find a video stream.");
    return -1;
  }
  return 0;
}

Video::~Video() {
  // Cleanup
  av_free(rgb_buffer);
  
  avcodec_free_context(&this->codec_context);
  avformat_close_input(&this->format_context);
  sws_freeContext(this->sws_context);

  for (auto frame : this->video_frames)
    SDL_DestroySurface(frame);

  delete this->frame;
}

int Video::Init(std::string & input_file) {

  int result = this->open_video_file(input_file);
  if (result != 0)
    return result;

  // get the video codec
  this->codec_parameters = format_context->streams[video_stream_index]->codecpar;
  this->codec = avcodec_find_decoder(this->codec_parameters->codec_id);
  if (!codec) {
    std::cerr << "Unsupported codec!\n";
    return -1;
  }

  this->codec_context = avcodec_alloc_context3(codec);
  avcodec_parameters_to_context(codec_context, this->codec_parameters);

  if (avcodec_open2(codec_context, codec, nullptr) < 0) {
    std::cerr << "Could not open codec.\n";
    return -1;
  }

  // Prepare frame structures
  this->frame = new Frame();

  // Setup SWS context for conversion
  this->sws_context = sws_getContext(
    codec_context->width,
    codec_context->height,
    codec_context->pix_fmt,
    codec_context->width,
    codec_context->height,
    AV_PIX_FMT_RGB24,
    SWS_BILINEAR,
    nullptr, nullptr, nullptr
  );

  int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_context->width, this->codec_context->height, 1);
  this->rgb_buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
  av_image_fill_arrays(frame->rgb_frame->data, frame->rgb_frame->linesize, rgb_buffer, AV_PIX_FMT_RGB24,
                       this->codec_context->width, this->codec_context->height, 1);

  // Read frames
  while (av_read_frame(format_context, frame->packet) >= 0) {
    if (
      frame->packet->stream_index == video_stream_index &&
      avcodec_send_packet(this->codec_context, frame->packet) == 0
    ) {
      while (avcodec_receive_frame(this->codec_context, frame->frame) == 0) {
        // Convert to RGB
        sws_scale(this->sws_context,
                  frame->frame->data, frame->frame->linesize, 0, this->codec_context->height,
                  frame->rgb_frame->data, frame->rgb_frame->linesize);
        // save all the pixel data in the frame
        int width = codec_context->width;
        int height = codec_context->height;
        int stride = frame->rgb_frame->linesize[0]; // how many bytes per row
                
        uint8_t * frame_data_raw = frame->rgb_frame->data[0];
        
        // Allocate output buffer (tight packed, no padding)
        uint8_t * rgb_output = (uint8_t*)SDL_malloc(height * width * 3);
        
        
        // Copy row by row
        for (int y = 0; y < height; y++) {
          uint8_t* src_row = frame_data_raw + y * stride;
          uint8_t* dst_row = rgb_output + y * width * 3;
          memcpy(dst_row, src_row, width * 3);  // copy only actual pixel data
        }
        
        SDL_PixelFormat pixel_fmt = SDL_PIXELFORMAT_RGB24;

        SDL_Surface* surface = SDL_CreateSurfaceFrom(
          width,        // Width of the image in pixels
          height,       // Height of the image in pixels
          pixel_fmt,    // The SDL_PixelFormat of your raw data (e.g., SDL_PIXELFORMAT_RGBA32)
          rgb_output,   // Pointer to your raw pixel data
          stride        // Bytes per row (width * bytes_per_pixel)
        );        
        
        this->video_frames.push_back(surface);

      }
    }
    av_packet_unref(frame->packet);
  }


  return 0;
}
