from PIL import Image
import os
import subprocess

def extract_media():
    # extract audio
    audio_cmd = ['ffmpeg', '-i', 'bad_apple.mp4', '-vn', '-acodec', 'pcm_s16le', 
                 '-ar', '44100', '-ac', '2', 'audio.wav']
    subprocess.run(audio_cmd)
    
    # extract frames
    frames_cmd = ['ffmpeg', '-i', 'bad_apple.mp4', '-vf', 'scale=180:70',
                  '-r', '30', 'frames/%d.png']
    subprocess.run(frames_cmd)

def convert_frames():
    frame_num = 1
    with open('frames/raw_frames.txt', 'wb') as outfile:
        while True:
            frame_path = f'frames/{frame_num}.png'
            if not os.path.exists(frame_path):
                break
                
            img = Image.open(frame_path).convert('L')
            img = img.resize((180, 70)) 
            
            outfile.write(img.tobytes())
            os.remove(frame_path)
            frame_num += 1
    
    print(f"Converted {frame_num-1} frames")
    print("cleaned up png files")

if __name__ == '__main__':
    os.makedirs('frames', exist_ok=True)
    
    extract_media()
    
    convert_frames()
