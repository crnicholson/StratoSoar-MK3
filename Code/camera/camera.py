# Instructions (these are all untested as of now!):
# 1. Add the video.bin file to this directory.
# 2. Modify the settings in this script as needed.
# 3. Run this script to extract the frames from the video file.
# 4. Your frames will now be in a folder called "frames" in this directory.
# 5. To covert the frames to a video, use the following command (you may need to chnange the framerate):
# ffmpeg -framerate 15 -i frames/frame_%04d.bmp -c:v libx264 -pix_fmt yuv420p video.mp4
# 6. You will now have a low quality MP4 video of the frames.

import os

cwd = os.getcwd()

bmp_header_size = 54  # Size of the BMP header in bytes.
frame_width = 160  # Width of the frame (for QQVGA).
frame_height = 120  # Height of the frame (for QQVGA).
frame_size = frame_width * frame_height * 2  # Each pixel is 2 bytes in RGB565 format.

video_file = f"{cwd}/video.bin"
output_dir = f"{cwd}/frames"


def extract_frames(video_file, output_dir):
    with open(video_file, "rb") as f:
        frame_number = 0
        while True:
            bmp_header = f.read(bmp_header_size)
            if len(bmp_header) < bmp_header_size:
                break

            frame_data = f.read(frame_size)
            if len(frame_data) < frame_size:
                break

            # Write the frame to a BMP file.
            frame_filename = os.path.join(output_dir, f"frame_{frame_number:04d}.bmp")
            with open(frame_filename, "wb") as frame_file:
                frame_file.write(bmp_header + frame_data)
            frame_number += 1


if __name__ == "__main__":
    # Create output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    extract_frames(video_file, output_dir)

    print(f"Extracted {len(os.listdir(output_dir))} frames to '{output_dir}'")
