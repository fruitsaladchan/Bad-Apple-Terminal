#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <SDL2/SDL.h>

#define WIDTH 180
#define HEIGHT 70
#define FRAME_SIZE (WIDTH * HEIGHT)
#define AUDIO_BUFFER_SIZE 4096
#define FPS 30
#define FRAME_TIME (1000000 / FPS)

SDL_AudioDeviceID audio_device;
Uint8* audio_buffer;
Uint32 audio_length;
Uint32 audio_pos = 0;
volatile int running = 1;
volatile int paused = 0;
struct timespec start_time, pause_time;
unsigned long total_pause_time = 0;
unsigned long frame_count = 0;

struct termios orig_termios;

void signal_handler(int signum) {
    running = 0;
}

void set_nonblocking_input() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void audio_callback(void* userdata, Uint8* stream, int len) {
    if (paused) {
        memset(stream, 0, len);
        return;
    }
    
    if (audio_pos + len > audio_length) {
        len = audio_length - audio_pos;
    }
    if (len > 0) {
        memcpy(stream, audio_buffer + audio_pos, len);
        audio_pos += len;
    }
}

const char* get_char(unsigned char brightness) {
    if (brightness < 51) return " ";
    if (brightness < 102) return "⠃";
    if (brightness < 153) return "⠇";
    if (brightness < 204) return "⠏";
    if (brightness < 230) return "⠟";
    return "⠿";
}

void clear_screen() {
    printf("\033[2J");
    printf("\033[H");
}

void move_cursor(int rows) {
    printf("\033[%dB", rows);
}

void reset_terminal_display() {
    printf("\033[2J");
    printf("\033[H");
    printf("\033[?25h");
}

int count_frames() {
    FILE* fp = fopen("frames/raw_frames.txt", "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size / FRAME_SIZE;
}

void render_frame(const unsigned char* frame_data) {
    clear_screen();
    move_cursor(2);
    
    if (paused) {
        printf("  [PAUSED] Press SPACE to resume\n");
    } else {
        printf("  Press SPACE to pause\n");
    }
    
    for (int y = 0; y < HEIGHT; y++) {
        printf("  ");
        for (int x = 0; x < WIDTH; x++) {
            printf("%s", get_char(frame_data[y * WIDTH + x]));
        }
        printf("\n");
    }
    fflush(stdout);
}

unsigned long get_microseconds() {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (current_time.tv_sec - start_time.tv_sec) * 1000000 + 
           (current_time.tv_nsec - start_time.tv_nsec) / 1000;
}

void check_keyboard_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) > 0) {
        if (c == ' ') {  // spacebar
            paused = !paused;
            if (paused) {
                clock_gettime(CLOCK_MONOTONIC, &pause_time);
            } else {
                struct timespec resume_time;
                clock_gettime(CLOCK_MONOTONIC, &resume_time);
                total_pause_time += (resume_time.tv_sec - pause_time.tv_sec) * 1000000 +
                                  (resume_time.tv_nsec - pause_time.tv_nsec) / 1000;
            }
        }
    }
}

int main() {
    signal(SIGINT, signal_handler);
    setlocale(LC_ALL, "en_US.UTF-8");
    printf("\033[?25l");
    
    set_nonblocking_input();

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec wav_spec;
    if (SDL_LoadWAV("audio.wav", &wav_spec, &audio_buffer, &audio_length) == NULL) {
        printf("Failed to load WAV file: %s\n", SDL_GetError());
        return 1;
    }

    wav_spec.callback = audio_callback;
    audio_device = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
    if (audio_device == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
        return 1;
    }

    FILE* fp = fopen("frames/raw_frames.txt", "rb");
    if (!fp) {
        printf("Failed to open frames file\n");
        return 1;
    }

    int total_frames = count_frames();
    printf("Total frames: %d\n", total_frames);
    
    unsigned char* frame_data = malloc(FRAME_SIZE);
    if (!frame_data) {
        printf("Memory allocation failed\n");
        fclose(fp);
        return 1;
    }

    usleep(150000);
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    SDL_PauseAudioDevice(audio_device, 0);

    while (running) {
        audio_pos = 0;
        frame_count = 0;
        total_pause_time = 0;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        for (int frame = 0; frame < total_frames && running; frame++) {
            check_keyboard_input();
            
            if (!paused) {
                unsigned long target_time = frame_count * FRAME_TIME;
                unsigned long current_time = get_microseconds() - total_pause_time;
                
                if (target_time > current_time) {
                    usleep(target_time - current_time);
                }

                fseek(fp, frame * FRAME_SIZE, SEEK_SET);
                if (fread(frame_data, 1, FRAME_SIZE, fp) != FRAME_SIZE) {
                    printf("Failed to read frame %d\n", frame);
                    break;
                }
                
                render_frame(frame_data);
                frame_count++;
            } else {
                render_frame(frame_data);
                usleep(33333);
                frame--;  
            }
        }
        
        if (running) {
            fseek(fp, 0, SEEK_SET);
        }
    }

    /* reset_terminal_display(); */
    restore_terminal();
    free(frame_data);
    fclose(fp);
    SDL_CloseAudioDevice(audio_device);
    SDL_FreeWAV(audio_buffer);
    SDL_Quit();
    return 0;
}
