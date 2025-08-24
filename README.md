# Bad Apple Terminal Player

Play bad apple in your terminal. Displays the animation using braile characters with synced audio playback, written in c

## Setup 

#### build

```bash
make all
```

#### build individually:
```bash
make frame-extract
make badapple
```

#### build and run after
```bash
make run  
```

#### clean up:
```bash
make clean        # Remove executables
make clean-media  # Remove extracted audio/frames
make clean-all    # Remove everything
```

#### install dependencies (Arch)
```bashk
make install-deps
```

## Notes
- The terminal window should be large enough to display 180x70 characters
- you can adjust size without editing the files by adusting terminal font size. Most terminal will use 'control +/-' to adjust size
- Make sure your terminal supports UTF-8 and Braille characters
- If the video seems out of sync, try adjusting the initial delay in the code (currently set to 150ms) and recompiling
