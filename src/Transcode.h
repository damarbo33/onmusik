/*
 * Copyright (c) 2013-2017 Andreas Unterweger
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* 
 * File:   Transcode.h
 * Author: dmarcobo
 *
 * Created on 7 de noviembre de 2017, 11:45
 */

#ifndef TRANSCODE_H
#define TRANSCODE_H

//NEEDED FOR FFMPEG AV INCLUDES!!!
//#define __STDC_FORMAT_MACROS
//#include <inttypes.h>
//NEEDED FOR FFMPEG AV INCLUDES!!!

#include <iostream>
#include <sstream>

using namespace std;

extern "C"{
    #include "libavformat/avformat.h"
    #include "libavformat/avio.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/audio_fifo.h"
    #include "libavutil/avassert.h"
    #include "libavutil/avstring.h"
    #include "libavutil/frame.h"
    #include "libavutil/opt.h"
    #include "libswresample/swresample.h"
}

/* The output bit rate in bit/s */
#define OUTPUT_BIT_RATE 128000
/* The number of output channels */
#define OUTPUT_CHANNELS 2

typedef struct AudioParams {
    int freq = 44100;
    int channels = OUTPUT_CHANNELS;
    int bitrate = OUTPUT_BIT_RATE;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

struct TID3Tags{
    string album;
    string title;
    string duration;
    string track;
    string genre;
    string publisher;
    string composer;
    string artist;
    string date;
};

class Transcode {
public:
    Transcode();
    Transcode(const Transcode& orig);
    virtual ~Transcode();
    int transcode(string orig, string dest);
    void setAudio_dst(AudioParams audio_dst);
    AudioParams getAudio_dst() const;
    TID3Tags getSongInfo(string filepath);
    int transcode(string orig, string dest, TID3Tags *tags);
  
    
private:
    //Buffer for errors added to c++ compatibility
    char buffErrors[AV_ERROR_MAX_STRING_SIZE];
    /* Global timestamp for the audio frames. */
    int64_t pts;
    struct AudioParams audio_dst;

    inline char *av_cplus_make_error_string(char *errbuf, size_t errbuf_size, int errnum);
    inline char *av_cplus_err2str(int errnum);
    int open_input_file(const char *filename,AVFormatContext **input_format_context,AVCodecContext **input_codec_context);
    int open_output_file(const char *filename,AVCodecContext *input_codec_context,AVFormatContext **output_format_context,AVCodecContext **output_codec_context);
    void init_packet(AVPacket *packet);
    int init_input_frame(AVFrame **frame);
    int init_resampler(AVCodecContext *input_codec_context,AVCodecContext *output_codec_context,SwrContext **resample_context);
    int init_fifo(AVAudioFifo **fifo, AVCodecContext *output_codec_context);
    int write_output_file_header(AVFormatContext *output_format_context);
    int decode_audio_frame(AVFrame *frame,AVFormatContext *input_format_context,AVCodecContext *input_codec_context,int *data_present, int *finished);
    int init_converted_samples(uint8_t ***converted_input_samples,AVCodecContext *output_codec_context,int frame_size);
    int convert_samples(const uint8_t **input_data,uint8_t **converted_data, const int frame_size,SwrContext *resample_context);
    int add_samples_to_fifo(AVAudioFifo *fifo,uint8_t **converted_input_samples,const int frame_size);
    int read_decode_convert_and_store(AVAudioFifo *fifo,AVFormatContext *input_format_context,AVCodecContext *input_codec_context,AVCodecContext *output_codec_context,SwrContext *resampler_context,int *finished);
    int init_output_frame(AVFrame **frame,AVCodecContext *output_codec_context,int frame_size);
    int encode_audio_frame(AVFrame *frame,AVFormatContext *output_format_context,AVCodecContext *output_codec_context,int *data_present);
    int load_encode_and_write(AVAudioFifo *fifo,AVFormatContext *output_format_context,AVCodecContext *output_codec_context);
    int write_output_file_trailer(AVFormatContext *output_format_context);
    
    int copy_metadata(AVFormatContext *input_format_context, AVFormatContext *output_format_context, TID3Tags *tags);
    void dump_metadata(void *ctx, AVDictionary *m, const char *indent);
    
};

#endif /* TRANSCODE_H */

