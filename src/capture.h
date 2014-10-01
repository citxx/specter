#pragma once

#include <vector>

#include <mutex>
#include <jack/jack.h>

class Capture {
 public:
  static int process(jack_nframes_t nframes, void *arg);
  static void jack_shutdown(void *arg);

  std::vector<float> buffer() const;

  static Capture* instance();

 private:
  Capture() = delete;
  Capture(jack_client_t *client, jack_port_t *input_port, int buffer_size);
  ~Capture();

  static Capture* make();

  void push_samples(jack_nframes_t nframes, jack_default_audio_sample_t *samples);

  static constexpr double _BUFFER_SIZE_SECS = 10;
  static Capture *_inst;

  mutable std::mutex _buffer_access;

  jack_port_t *_input_port;
  jack_client_t *_client;

  std::vector<float> _buffer;
  int _head;
};
