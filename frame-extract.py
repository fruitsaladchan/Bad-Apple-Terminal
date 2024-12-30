from PIL import Image
import os

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
            frame_num += 1
    
    print(f"Converted {frame_num-1} frames")

if __name__ == '__main__':
    convert_frames()
