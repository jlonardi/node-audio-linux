const { bgWhite } = require('chalk');
const readline = require('readline');
const { volume } = require("./");
const { getVolume, setVolume, isMuted, setMute } = volume;


console.log('');
console.log('up/right   - volume up');
console.log('down/left  - volume down');
console.log('m          - mute/unmute');
console.log('esc        - quit');
console.log('');




readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);

process.stdin.on('keypress', (str, key) => {
  switch (key.name) {
    case 'escape':
      process.exit();
    case 'right':
    case 'up': {
      setVolume(getVolume() + 0.02);
      break;
    }
    case 'down':
    case 'left':
      setVolume(getVolume() - 0.02);
      break;
    case 'm': {
      setMute(true);
      // setMute(!isMuted());
      break;
    }
  }
  drawBar(getVolume());
});


const getBar = (length, char) => {
  let str = '';
  for (let i = 0; i < length; i += 1) {
    str += char;
  }
  return str;
};

const drawBar = (current) => {
  const barLength = 50;
  const percentageProgress = (current * 100).toFixed(0);
  this.currentProgress = percentageProgress;

  // const filledBarLength = isMuted() ? 0 : (current * barLength).toFixed(0);
  const filledBarLength = (current * barLength).toFixed(0);


  const emptyBarLength = barLength - filledBarLength;

  const filledBar = bgWhite(getBar(filledBarLength, ' '));
  const emptyBar = getBar(emptyBarLength, '·');
  const title = 'Volume: ';
  let emoticon = '\u{1F50A}';
  if (percentageProgress < 40) {
    emoticon = '\u{1F509}';
  }
  if (percentageProgress < 10) {
    emoticon = '\u{1F508}';
  }
  // if (isMuted()) {
  //   emoticon = '\u{1F507}';
  // }

  process.stdout.clearLine();
  process.stdout.cursorTo(0);
  process.stdout.write(`${title}  ${emoticon}  [${filledBar}${emptyBar}] | ${percentageProgress}%            `);
}

drawBar(getVolume());