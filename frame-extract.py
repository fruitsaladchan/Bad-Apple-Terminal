from PIL import Image
import os
import subprocess
from rich import print
from rich.progress import track
from rich.console import Console

console = Console()

def extract_media():
    with console.status("[bold green]Extracting media...", spinner="dots"):
        # extract audio
        audio_cmd = ['ffmpeg', '-i', 'bad_apple.mp4', '-vn', '-acodec', 'pcm_s16le', 
                    '-ar', '44100', '-ac', '2', 'audio.wav']
        subprocess.run(audio_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        console.print("[green]✓[/green] Audio extracted")
        
        # extract frames
        frames_cmd = ['ffmpeg', '-i', 'bad_apple.mp4', '-vf', 'scale=180:70',
                    '-r', '30', 'frames/%d.png']
        subprocess.run(frames_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        console.print("[green]✓[/green] Frames extracted")

def convert_frames():
    frame_num = 1
    total_frames = len([f for f in os.listdir('frames') if f.endswith('.png')])
    
    with console.status("[bold blue]Converting frames...", spinner="dots"):
        with open('frames/raw_frames.txt', 'wb') as outfile:
            for _ in track(range(total_frames), description="Processing frames"):
                frame_path = f'frames/{frame_num}.png'
                if not os.path.exists(frame_path):
                    break
                    
                img = Image.open(frame_path).convert('L')
                img = img.resize((180, 70)) 
                
                outfile.write(img.tobytes())
                os.remove(frame_path)
                frame_num += 1
        
        console.print(f"[green]✓[/green] Converted [bold cyan]{frame_num-1}[/bold cyan] frames")
        console.print("[green]✓[/green] Cleaned up PNG files")

if __name__ == '__main__':
    console.print("[bold yellow]Starting media extraction process...[/bold yellow]")
    os.makedirs('frames', exist_ok=True)
    
    extract_media()
    convert_frames()
    
    console.print("\n[bold green]All operations completed successfully! [/bold green]")
