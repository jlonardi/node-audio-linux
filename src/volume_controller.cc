#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <iostream>
#include <nan.h>

typedef enum {
    AUDIO_VOLUME_SET,
    AUDIO_VOLUME_GET,
    AUDIO_MUTE,
    AUDIO_UNMUTE,
} audio_volume_action;

int audio(audio_volume_action action, long* outvol)
{

  long min, max;
  long volume = 0;
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  const char *card = "default";
  const char *selem_name = "Master";

  snd_mixer_open(&handle, 0);
  snd_mixer_attach(handle, card);
  snd_mixer_selem_register(handle, NULL, NULL);
  snd_mixer_load(handle);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, selem_name);

  snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  if (action == AUDIO_VOLUME_GET) {
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);
    *outvol = 100 * volume / max;
    // std::cout << "Get volume: " << *outvol << std::endl;
  }

  if (action == AUDIO_VOLUME_SET) {
    *outvol = (*outvol * (max - min) / (100-1)) + min;
    // std::cout << "Set volume: " << *outvol << std::endl;
    snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, *outvol);
    snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_RIGHT, *outvol);
  }

  int muted = 1;
  snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &muted);

  if (action == AUDIO_MUTE) {
    snd_mixer_selem_set_playback_switch_all(elem, 0);
  }

  if (action == AUDIO_UNMUTE) {
    snd_mixer_selem_set_playback_switch_all(elem, 1);
  }


  snd_mixer_close(handle);

  return 0;
}


void setVolume(float vol) {
  float newVolume = vol;


  if (newVolume > 1.0) {
    newVolume = 1.0;
  }

  if (newVolume < 0) {
    newVolume = 0;
  }

  long outvol = newVolume * 100;

  // std::cout << "Got in value " << vol << std::endl;
  // std::cout << "Outvolume is set to " << outvol << std::endl;
  audio(AUDIO_VOLUME_SET, &outvol);
}

float getVolume () {
  long currentVolume = 0;

  audio(AUDIO_VOLUME_GET, &currentVolume);
  // std::cout << "Current volume " << currentVolume << std::endl;
  // std::cout << "Returning volume " << (currentVolume / 100.f) << std::endl;

  return (float) (currentVolume / 100.f);
}

// bool isMuted() {
//   BOOL muted = false;

//   IAudioEndpointVolume *endpointVolume = getVolumeCOM();

//   checkErrors(endpointVolume->GetMute(&muted), "getting muted state");

//   return muted;
// }

bool setMute(bool mute) {
  long currentVolume = 0;
  audio(AUDIO_MUTE, &currentVolume);
  return mute;
  // if (isMuted() && mute) {
  //   return mute;
  // }

  // IAudioEndpointVolume *endpointVolume = getVolumeCOM();

  // checkErrors(endpointVolume->SetMute(mute, NULL), "setting mute");

  // return mute;
}

NAN_METHOD(GetVolume) {

  info.GetReturnValue().Set(Nan::New(getVolume()));
}

NAN_METHOD(SetVolume) {
  float newVolume = (float)info[0]->NumberValue();
  setVolume(newVolume);
  // info.GetReturnValue().Set(Nan::New(setVolume(newVolume)));
}

// NAN_METHOD(IsMuted) {
//   info.GetReturnValue().Set(Nan::New(isMuted()));
// }

NAN_METHOD(SetMute) {
  bool muted = info[0]->BooleanValue();
  setMute(muted);
  // info.GetReturnValue().Set(Nan::New(setMute(muted)));
}

NAN_MODULE_INIT(init) {
  target->Set(  Nan::New("getVolume").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(GetVolume)->GetFunction());
  target->Set(  Nan::New("setVolume").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(SetVolume)->GetFunction());
  // target->Set(  Nan::New("isMuted").ToLocalChecked(),
  //               Nan::New<v8::FunctionTemplate>(IsMuted)->GetFunction());
  target->Set(  Nan::New("setMute").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(SetMute)->GetFunction());
}

NODE_MODULE(addon, init)