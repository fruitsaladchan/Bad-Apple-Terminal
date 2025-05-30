# Bad Apple Terminal Player

Play bad apple in your terminal. Displays the animation using braile characters with synced audio playback, written in c

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
pip install image rich
```

## Setup 

1. Extract frames and audio from the video:
```bash
python ./frame-extract.py
```

2. Compile :
```bash
gcc -o badapple badapple.c -lSDL2 $(sdl2-config --cflags)
```

## Notes
- The terminal window should be large enough to display 180x70 characters
- You can change the resolution by editing ./badapple.c & ./frame-extract.py (line 19 and 36 and ) and rerunning all the steps
-  you can adjust size without editing the files by adusting terminal font size. Most terminal will use 'control +/-' to adjust size
- Make sure your terminal supports UTF-8 and Braille characters
- If the video seems out of sync, try adjusting the initial delay in the code (currently set to 150ms) and recompiling
