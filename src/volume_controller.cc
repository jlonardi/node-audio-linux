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

snd_mixer_t *handle;

snd_mixer_elem_t* getMixerElem() {

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
  return elem;
}

float getVolume() {
  long min, max;
  long volume = 0;

  snd_mixer_elem_t* elem = getMixerElem();

  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);

  snd_mixer_close(handle);

  return volume / (1.f * max);
}

// volume 0.0 - 1.0
void setVolumeLevel(float volume) {

  long outvol = volume * 100;
  long min, max;

  snd_mixer_elem_t* elem = getMixerElem();

  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  outvol = outvol * (max - min) / (100-1) + min;
  snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, outvol);
  snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_RIGHT, outvol);

  snd_mixer_close(handle);
}


void setMuteState(bool muted) {
  snd_mixer_elem_t* elem = getMixerElem();

  if (muted) {
    snd_mixer_selem_set_playback_switch_all(elem, 0);
  } else {
    snd_mixer_selem_set_playback_switch_all(elem, 1);
  }
  snd_mixer_close(handle);
}

bool isMuted() {
  int leftMuted = 0;
  int rightMuted = 0;

  snd_mixer_elem_t* elem = getMixerElem();

  snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &leftMuted);
  snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_RIGHT, &rightMuted);

  snd_mixer_close(handle);
  return leftMuted + rightMuted == 0;
}

float setVolume(float vol) {
  float newVolume = vol;


  if (newVolume > 1.0) {
    newVolume = 1.0;
  }

  if (newVolume < 0) {
    newVolume = 0;
  }

  setVolumeLevel(newVolume);
  return getVolume();
}

bool setMute(bool muted) {
  setMuteState(muted);
  return muted;
}

NAN_METHOD(GetVolume) {

  info.GetReturnValue().Set(Nan::New(getVolume()));
}

NAN_METHOD(SetVolume) {
  float newVolume = (float)info[0]->NumberValue();
  setVolume(newVolume);
  // info.GetReturnValue().Set(Nan::New(setVolume(newVolume)));
}

NAN_METHOD(IsMuted) {
  info.GetReturnValue().Set(Nan::New(isMuted()));
}

NAN_METHOD(SetMute) {
  bool muted = info[0]->BooleanValue();
  setMute(muted);
  info.GetReturnValue().Set(Nan::New(isMuted()));
}

NAN_MODULE_INIT(init) {
  target->Set(  Nan::New("getVolume").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(GetVolume)->GetFunction());
  target->Set(  Nan::New("setVolume").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(SetVolume)->GetFunction());
  target->Set(  Nan::New("isMuted").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(IsMuted)->GetFunction());
  target->Set(  Nan::New("setMute").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(SetMute)->GetFunction());
}

NODE_MODULE(addon, init)