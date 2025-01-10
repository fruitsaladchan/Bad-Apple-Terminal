from PIL import Image
import os
import subprocess
from rich import print
from rich.progress import track
from rich.console import Console

console = Console()

def extract_media():
    with console.status("[bold green]Extracting media...", spinner="dots"):
        try:
            # extract audio
            audio_cmd = ['ffmpeg', '-i', 'bad_apple.mp4', '-vn', '-acodec', 'pcm_s16le', 
                        '-ar', '44100', '-ac', '2', 'audio.wav']
            audio_process = subprocess.run(
                audio_cmd + ['-y', '-hide_banner'],
                capture_output=True,
                text=True,
                check=True
            )
            console.print("[green]✓[/green] Audio extracted")
            
            # extract frames
            frames_cmd = ['ffmpeg', '-i', 'bad_apple.mp4', '-vf', 'scale=180:70',
                        '-r', '30', 'frames/%d.png']
            frames_process = subprocess.run(
                frames_cmd + ['-y', '-hide_banner'],
                capture_output=True,
                text=True,
                check=True
            )
            console.print("[green]✓[/green] Frames extracted")
            
        except subprocess.CalledProcessError as e:
            console.print(f"[red]Error during media extraction:[/red] {str(e)}")
            console.print(f"[red]Error output:[/red] {e.stderr}")
            raise

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
