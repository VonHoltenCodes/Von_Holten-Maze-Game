#!/usr/bin/env python3
"""
Convert sprites from PNG to VGA C arrays for DOS game
"""
from PIL import Image
import sys
import os

# VGA color palette (256 colors, first 16 are standard)
VGA_PALETTE = [
    (0, 0, 0),       # 0 - Black
    (0, 0, 170),     # 1 - Blue
    (0, 170, 0),     # 2 - Green
    (0, 170, 170),   # 3 - Cyan
    (170, 0, 0),     # 4 - Red
    (170, 0, 170),   # 5 - Magenta
    (170, 85, 0),    # 6 - Brown
    (170, 170, 170), # 7 - Light Gray
    (85, 85, 85),    # 8 - Dark Gray
    (85, 85, 255),   # 9 - Light Blue
    (85, 255, 85),   # 10 - Light Green
    (85, 255, 255),  # 11 - Light Cyan
    (255, 85, 85),   # 12 - Light Red
    (255, 85, 255),  # 13 - Light Magenta
    (255, 255, 85),  # 14 - Yellow
    (255, 255, 255), # 15 - White
]

# Extended grayscale for better gradients
for i in range(16, 32):
    g = int((i - 16) * 255 / 15)
    VGA_PALETTE.append((g, g, g))

# Extended colors for sprites (browns, greens, blues)
for i in range(32, 256):
    r = (i * 37) % 256
    g = (i * 67) % 256
    b = (i * 97) % 256
    VGA_PALETTE.append((r, g, b))

def find_closest_vga_color(r, g, b, alpha=255):
    """Find closest VGA color for RGB value"""
    if alpha < 128:  # Transparent
        return 0

    min_dist = float('inf')
    closest = 0

    for i, (vr, vg, vb) in enumerate(VGA_PALETTE[:256]):
        dist = (r - vr)**2 + (g - vg)**2 + (b - vb)**2
        if dist < min_dist:
            min_dist = dist
            closest = i

    return closest

def convert_image_to_vga(img_path, target_width, target_height):
    """Convert image to VGA color array"""
    # Load and resize image
    img = Image.open(img_path).convert('RGBA')
    img = img.resize((target_width, target_height), Image.Resampling.LANCZOS)

    # Convert to VGA colors
    pixels = img.load()
    vga_data = []

    for y in range(target_height):
        for x in range(target_width):
            r, g, b, a = pixels[x, y]
            vga_color = find_closest_vga_color(r, g, b, a)
            vga_data.append(vga_color)

    return vga_data

def generate_c_header(name, width, height, data, output_path):
    """Generate C header file with sprite data"""
    with open(output_path, 'w') as f:
        f.write(f"/* {name} sprite - {width}x{height} */\n")
        f.write(f"#define {name.upper()}_WIDTH {width}\n")
        f.write(f"#define {name.upper()}_HEIGHT {height}\n")
        f.write(f"static unsigned char {name}[{len(data)}] = {{\n")

        # Write data in rows
        for y in range(height):
            f.write("    /* Row {} */\n    ".format(y))
            for x in range(width):
                idx = y * width + x
                f.write(str(data[idx]))
                if idx < len(data) - 1:
                    f.write(",")
                if (x + 1) % 20 == 0 and x < width - 1:
                    f.write("\n    ")
            f.write("\n")

        f.write("};\n")

if __name__ == "__main__":
    base_path = "/home/vonholten/Von_Holten-Maze-Game"

    print("Converting better weapon sprite...")
    pistol_data = convert_image_to_vga(
        f"{base_path}/maze_sprites/FPS Weapon Sprites/frames/pistol1.png",
        60, 40
    )
    generate_c_header("weapon_pistol_better", 60, 40, pistol_data,
                     f"{base_path}/src/weapon_pistol_better.h")
    print("  ✓ weapon_pistol_better.h created (60x40)")

    print("\nConverting enemy sprites...")
    # Find soldier and zombie sprites
    soldier_files = os.listdir(f"{base_path}/maze_sprites/PNG/Soldier 1")
    zombie_files = os.listdir(f"{base_path}/maze_sprites/PNG/Zombie 1")

    # Get first soldier sprite
    soldier_img = [f for f in soldier_files if f.endswith('.png')][0]
    soldier_data = convert_image_to_vga(
        f"{base_path}/maze_sprites/PNG/Soldier 1/{soldier_img}",
        32, 32
    )
    generate_c_header("enemy_soldier_better", 32, 32, soldier_data,
                     f"{base_path}/src/enemy_soldier_better.h")
    print(f"  ✓ enemy_soldier_better.h created (32x32) from {soldier_img}")

    # Get first zombie sprite
    zombie_img = [f for f in zombie_files if f.endswith('.png')][0]
    zombie_data = convert_image_to_vga(
        f"{base_path}/maze_sprites/PNG/Zombie 1/{zombie_img}",
        32, 32
    )
    generate_c_header("enemy_zombie_better", 32, 32, zombie_data,
                     f"{base_path}/src/enemy_zombie_better.h")
    print(f"  ✓ enemy_zombie_better.h created (32x32) from {zombie_img}")

    print("\nAll sprites converted successfully!")
