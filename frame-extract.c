#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize2.h"

// Function to create directory if it doesn't exist
int create_directory(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) == -1) {
            perror("mkdir");
            return -1;
        }
    }
    return 0;
}

// Function to execute system command and check result
int execute_command(const char* command) {
    printf("Executing: %s\n", command);
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Command failed with exit code: %d\n", result);
        return -1;
    }
    return 0;
}

// Function to extract media using ffmpeg
int extract_media() {
    printf("Extracting media...\n");
    
    // Extract audio
    const char* audio_cmd = "ffmpeg -i bad_apple.mp4 -vn -acodec pcm_s16le -ar 44100 -ac 2 audio.wav -y -hide_banner";
    if (execute_command(audio_cmd) != 0) {
        fprintf(stderr, "Failed to extract audio\n");
        return -1;
    }
    printf("✓ Audio extracted\n");
    
    // Extract frames
    const char* frames_cmd = "ffmpeg -i bad_apple.mp4 -vf scale=180:70 -r 30 frames/%d.png -y -hide_banner";
    if (execute_command(frames_cmd) != 0) {
        fprintf(stderr, "Failed to extract frames\n");
        return -1;
    }
    printf("✓ Frames extracted\n");
    
    return 0;
}

// Function to count PNG files in frames directory
int count_png_files() {
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    
    dir = opendir("frames");
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".png") != NULL) {
            count++;
        }
    }
    
    closedir(dir);
    return count;
}

// Function to convert RGB to grayscale
void rgb_to_grayscale(unsigned char* rgb_data, unsigned char* gray_data, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        if (channels == 3) {
            // RGB to grayscale using standard luminance weights
            gray_data[i] = (unsigned char)(0.299 * rgb_data[i*3] + 
                                         0.587 * rgb_data[i*3+1] + 
                                         0.114 * rgb_data[i*3+2]);
        } else if (channels == 4) {
            // RGBA to grayscale
            gray_data[i] = (unsigned char)(0.299 * rgb_data[i*4] + 
                                         0.587 * rgb_data[i*4+1] + 
                                         0.114 * rgb_data[i*4+2]);
        } else if (channels == 1) {
            // Already grayscale
            gray_data[i] = rgb_data[i];
        }
    }
}

// Function to convert frames
int convert_frames() {
    printf("Converting frames...\n");
    
    int total_frames = count_png_files();
    if (total_frames <= 0) {
        fprintf(stderr, "No PNG files found or error counting files\n");
        return -1;
    }
    
    FILE* outfile = fopen("frames/raw_frames.txt", "wb");
    if (outfile == NULL) {
        perror("fopen");
        return -1;
    }
    
    int frame_num = 1;
    int processed = 0;
    
    while (frame_num <= total_frames) {
        char frame_path[256];
        snprintf(frame_path, sizeof(frame_path), "frames/%d.png", frame_num);
        
        // Check if file exists
        if (access(frame_path, F_OK) != 0) {
            frame_num++;
            continue;
        }
        
        // Load image
        int width, height, channels;
        unsigned char* img_data = stbi_load(frame_path, &width, &height, &channels, 0);
        if (img_data == NULL) {
            fprintf(stderr, "Failed to load image: %s\n", frame_path);
            frame_num++;
            continue;
        }
        
        // Resize if necessary
        unsigned char* resized_data = NULL;
        if (width != 180 || height != 70) {
            resized_data = malloc(180 * 70 * channels);
            if (resized_data == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                stbi_image_free(img_data);
                break;
            }
            
            if (!stbir_resize_uint8_linear(img_data, width, height, width * channels,
                                          resized_data, 180, 70, 180 * channels,
                                          (stbir_pixel_layout)channels)) {
                fprintf(stderr, "Failed to resize image: %s\n", frame_path);
                free(resized_data);
                stbi_image_free(img_data);
                frame_num++;
                continue;
            }
            
            stbi_image_free(img_data);
            img_data = resized_data;
            width = 180;
            height = 70;
        }
        
        // Convert to grayscale
        unsigned char* gray_data = malloc(width * height);
        if (gray_data == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            if (resized_data) free(resized_data);
            else stbi_image_free(img_data);
            break;
        }
        
        rgb_to_grayscale(img_data, gray_data, width, height, channels);
        
        // Write to output file
        fwrite(gray_data, 1, width * height, outfile);
        
        // Clean up
        if (resized_data) free(resized_data);
        else stbi_image_free(img_data);
        free(gray_data);
        
        // Remove the PNG file
        if (remove(frame_path) != 0) {
            perror("remove");
        }
        
        processed++;
        frame_num++;
        
        // Simple progress indicator
        if (processed % 100 == 0 || processed == total_frames) {
            printf("Processed %d/%d frames\n", processed, total_frames);
        }
    }
    
    fclose(outfile);
    
    printf("✓ Converted %d frames\n", processed);
    printf("✓ Cleaned up PNG files\n");
    
    return 0;
}

int main() {
    printf("Starting media extraction process...\n");
    
    // Create frames directory
    if (create_directory("frames") != 0) {
        return 1;
    }
    
    // Extract media
    if (extract_media() != 0) {
        return 1;
    }
    
    // Convert frames
    if (convert_frames() != 0) {
        return 1;
    }
    
    printf("\nAll operations completed successfully!\n");
    
    return 0;
}
