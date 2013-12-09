// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cassert>
#include <cmath>
#include <limits>
#include <sstream>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_array_buffer.h"

namespace audio_effects {

const char* const kBufferMessageId = "BF";

class AudioEffectsInstance : public pp::Instance {
 public:
  explicit AudioEffectsInstance(PP_Instance instance)
      : pp::Instance(instance) {}
  virtual ~AudioEffectsInstance() {}

  virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
  virtual void HandleMessage(const pp::Var& var_message);

 private:
  typedef enum {
    STATE_MESSAGE,
    STATE_BUFFER
  } State;

  void HandleStringMessage(const std::string& message);
  void HandleArrayBufMessage(const pp::VarArrayBuffer& message);

  State state;
};

bool AudioEffectsInstance::Init(uint32_t argc,
                             const char* argn[],
                             const char* argv[]) {
  state = STATE_MESSAGE;
  return true;
}

void AudioEffectsInstance::HandleMessage(const pp::Var& var_message) {
  if (var_message.is_string()) {
    HandleStringMessage(var_message.AsString());
  } else if (var_message.is_array_buffer()) {
    pp::VarArrayBuffer buffer(var_message);
    HandleArrayBufMessage(buffer);
  } else {
    LogToConsole(PP_LOGLEVEL_ERROR, pp::Var("Did not understand message"));
  }
}

void AudioEffectsInstance::HandleStringMessage(const std::string& message) {
  if (state != STATE_MESSAGE)
    return;

  if (message == kBufferMessageId) {
    state = STATE_BUFFER;
  }
}

void AudioEffectsInstance::HandleArrayBufMessage(
    const pp::VarArrayBuffer& message) {
  if (state != STATE_BUFFER)
    return;

  PostMessage(message);

  state = STATE_BUFFER;
}

class AudioEffectsModule : public pp::Module {
 public:
  AudioEffectsModule() : pp::Module() {}
  ~AudioEffectsModule() {}

  // Create and return a HelloWorldInstance object.
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new AudioEffectsInstance(instance);
  }
};

}  // namespace audio_effects

namespace pp {
Module* CreateModule() { return new audio_effects::AudioEffectsModule(); }
}  // namespace pp
