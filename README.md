# Bad Apple Terminal Player

Play bad apple in your terminal. Displays the animation using braile characters with synchronized autio playback, written in c


## Features
- ASCII art animation using Braille characters
- Synchronized audio playback
- Clean terminal handling
- 30 FPS playback
- exit with Ctrl+C

## Dependencies

Install the required dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install ffmpeg libsdl2-dev imagemagick

# Fedora
sudo dnf install ffmpeg SDL2-devel ImageMagick

# Arch Linux
sudo pacman -S ffmpeg sdl2 imagemagick
```

##### Needed for the python script
```bash
pip install image
```

## Setup Instructions

1. Extract audio from the video:
```bash
ffmpeg -i bad_apple.mp4 -vn -acodec pcm_s16le -ar 44100 -ac 2 audio.wav
```

2. Extract frames from the video:
```bash
ffmpeg -i bad_apple.mp4 -vf scale=180:70 -r 30 frames/%d.png
```

3. Convert frames to grayscale raw format:
```bash
python ./frame-extract.py
```

4. Compile :
```bash
gcc -o badapple badapple.c -lSDL2 $(sdl2-config --cflags)
```

5. Run the player:
```bash
./badapple
```

## Notes
- The terminal window should be large enough to display 180x70 characters
- You can change the resilution by editing ./badapple.c & ./frame-extract.py and rerunning all the steps
- You can delete the .png files in the frames directory after youve run the python converter
- Make sure your terminal supports UTF-8 and Braille characters
- The audio file (audio.wav) should be in the same directory as the player
- If the video seems out of sync, try adjusting the initial delay in the code (currently set to 150ms)

## Troubleshooting

1. If you see no output:
   - Check if your terminal supports UTF-8: `echo "⠿"`
   - Try running with explicit locale: `LANG=en_US.UTF-8 ./badapple`

2. If audio doesn't play:
   - Verify audio.wav exists in the correct directory
   - Check SDL2 is installed correctly
   - Check audio device permissions

3. If the video is too large/small:
   - Adjust your terminal font size. Most terminal will use control +/- to adjust size
   - Or modify the scale values in the ffmpeg command (step 4)

## File Structure
```
.
├── badapple.c
├── badapple
├── badapple.mp4
├── frame-extract.py
├── audio.wav
└── frames/
    ├── raw_frames.txt
    └── [png files]
```
