# Native Node.js volume controls for Linux

A native binding to [ALSA](https://www.alsa-project.org/main/index.php/Main_Page) to control the default audio device on Linux. Supports set/get the volume and get the muted state and set the muted state.

[![npm](https://img.shields.io/npm/dm/node-audio-linux.svg)](https://www.npmjs.com/package/node-audio-linux)
[![npm](https://img.shields.io/npm/v/node-audio-linux.svg)](https://www.npmjs.com/package/node-audio-linux)

## How to Use
```javascript
const { volume } = require('node-volume-linux');

// the functions the volume controller exposes
const { getVolume, setVolume, isMuted, setMute } = volume;

// to get the system volume
const currentVolume = getVolume();

// to set the volume to 43.
setVolume(0.43);

// to know if the system is muted
isMuted();

// to mute
setMute(true);

// to unmute
setMute(false);

// to toggle mute
setMute(!isMuted());
```
#### Note
Alsa takes as it's volume input a value between the mixer's min and max values which are not in the `0-1.0` scale. For this two volume sets with the value in the range of `0 < x < 0.02` might yield the same volume level. Setting a value grater `x >= 0.02` does change the volume up. This might be an issue when implementing a for example volume sliders.

## Development
The native module build depends on the package [libasound2-dev](https://packages.debian.org/fi/sid/libasound2-dev). The build also uses pkg-config. To get the needed dependencies.

```bash
$ sudo apt-get install libasound2-dev pkg-config
```

After this you can build

```bash
$ node-gyp configure
$ node-gyp build
```
The repository contains a demo of the usage
```bash
$ node demo.js
```


## Next steps
 - Expose the microphone API

## Licence & Copyright
Copyright (c) 2018 Jarno Lonardi
The package `node-audio-linux` is licensed under MIT license.