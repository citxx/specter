#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>

#include <mutex>
#include <jack/jack.h>

#include "capture.h"


Capture *Capture::_inst = nullptr;


int Capture::process(jack_nframes_t nframes, void *arg) {
  jack_default_audio_sample_t *in;
  
  in = static_cast<jack_default_audio_sample_t*>(
      jack_port_get_buffer(instance()->_input_port, nframes));
  instance()->push_samples(nframes, in);

  return 0;      
}

void Capture::jack_shutdown(void *arg) {
  if (_inst != nullptr) {
    delete _inst;
    _inst = nullptr;
  }
}

Capture* Capture::instance() {
  if (_inst == nullptr) {
    _inst = Capture::make();
  }
  return _inst;
}

Capture::Capture(
    jack_client_t *client,
    jack_port_t *input_port,
    int buffer_size):
  _client(client),
  _input_port(input_port),
  _buffer(buffer_size, 0),
  _head(0) {
}

Capture::~Capture() {
  jack_client_close(_client);
}

Capture* Capture::make() {
  const char *client_name = "specter";
  const char *server_name = nullptr;
  jack_options_t options = JackNullOption;
  jack_status_t status;
  
  jack_client_t *client = jack_client_open(client_name, options, &status, server_name);
  if (client == nullptr) {
    fprintf(stderr, "jack_client_open() failed, "
       "status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf(stderr, "Unable to connect to JACK server\n");
    }
    return nullptr;
  }
  if (status & JackServerStarted) {
    fprintf(stderr, "JACK server started\n");
  }
  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf (stderr, "unique name `%s' assigned\n", client_name);
  }

  jack_set_process_callback(client, Capture::process, 0);
  jack_on_shutdown(client, Capture::jack_shutdown, 0);

  unsigned int sr = jack_get_sample_rate(client);
  printf("engine sample rate: %" PRIu32 "\n", sr);

  jack_port_t *input_port = jack_port_register(client, "input",
           JACK_DEFAULT_AUDIO_TYPE,
           JackPortIsInput, 0);

  if (input_port == NULL) {
    fprintf(stderr, "no more JACK ports available\n");
    jack_client_close(client);
    return nullptr;
  }

  Capture *capture = new Capture(
      client, input_port, floor(sr * _BUFFER_SIZE_SECS));
  if (jack_activate(client)) {
    fprintf(stderr, "cannot activate client");
  }

  const char **ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
  if (ports == NULL) {
    fprintf(stderr, "no physical capture ports\n");
  }
  if (jack_connect(client, ports[0], jack_port_name(input_port))) {
    fprintf(stderr, "cannot connect input ports\n");
  }
  free(ports);

  return capture;
}

void Capture::push_samples(
    jack_nframes_t nframes,
    jack_default_audio_sample_t *samples) {
  _buffer_access.lock();
  for (int i = 0; i < nframes; ++i) {
    _buffer[_head] = samples[i];
    _head += 1;
    if (_head == _buffer.size()) {
      _head = 0;
    }
  }
  _buffer_access.unlock();
}

std::vector<float> Capture::buffer() const {
  _buffer_access.lock();
  std::vector<float> result(_buffer.size());
  for (int i = 0; i < (int)result.size(); ++i) {
    result[i] = _buffer[(_head + i) % _buffer.size()];
  }
  _buffer_access.unlock();
  return result;
}
