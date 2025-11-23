#!/usr/bin/env python3
"""
Convert new sprite set to VGA format for DOS game
Includes enemies, weapon, floor, and wall textures
"""

from PIL import Image
import sys
import os

# Standard 16 VGA colors (Mode 13h)
VGA_COLORS = [
    (0, 0, 0),       # 0 - Black
    (0, 0, 170),     # 1 - Blue
    (0, 170, 0),     # 2 - Green
    (0, 170, 170),   # 3 - Cyan
    (170, 0, 0),     # 4 - Red
    (170, 0, 170),   # 5 - Magenta
    (170, 85, 0),    # 6 - Brown
    (170, 170, 170), # 7 - White/Light Gray
    (85, 85, 85),    # 8 - Dark Gray
    (85, 85, 255),   # 9 - Light Blue
    (85, 255, 85),   # 10 - Light Green
    (85, 255, 255),  # 11 - Light Cyan
    (255, 85, 85),   # 12 - Light Red
    (255, 85, 255),  # 13 - Light Magenta
    (255, 255, 85),  # 14 - Yellow
    (255, 255, 255)  # 15 - Bright White
]

def find_closest_vga_color(r, g, b, alpha):
    """Find the closest VGA color to the given RGB value"""
    # If transparent (alpha < 128), return 0 (transparent in our engine)
    if alpha < 128:
        return 0

    # Special handling for purple/magenta tones
    if b > g + 30 and b > r - 30:
        if r > 200 and b > 200:  # Light purple
            return 13  # Light Magenta
        else:  # Dark purple
            return 5   # Magenta

    # Special handling for green tones (creeper)
    if g > r + 30 and g > b + 30:
        if g > 200:  # Light green
            return 10  # Light Green
        else:  # Dark green
            return 2   # Green

    # Special handling for red/orange tones (tomato, spiderman)
    if r > g + 30 and r > b + 30:
        if r > 200:  # Light red/orange
            return 12  # Light Red
        elif r > 150:
            return 4   # Red
        else:
            return 6   # Brown

    # Find closest color using Euclidean distance
    min_dist = float('inf')
    closest_idx = 0

    for idx, (vga_r, vga_g, vga_b) in enumerate(VGA_COLORS):
        dist = ((r - vga_r) ** 2 + (g - vga_g) ** 2 + (b - vga_b) ** 2) ** 0.5
        if dist < min_dist:
            min_dist = dist
            closest_idx = idx

    return closest_idx

def convert_sprite(input_path, output_name, target_width=32, target_height=32):
    """Convert PNG/GIF sprite to VGA C header file"""
    print(f"Converting {input_path}...")

    # Load image
    img = Image.open(input_path)

    # Convert to RGBA if not already
    if img.mode != 'RGBA':
        img = img.convert('RGBA')

    # Resize if needed (using NEAREST to preserve pixel art)
    if img.size != (target_width, target_height):
        print(f"  Resizing from {img.size} to {target_width}x{target_height}")
        img = img.resize((target_width, target_height), Image.NEAREST)

    width, height = img.size
    pixels = img.load()

    # Convert to VGA colors
    vga_data = []
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            vga_color = find_closest_vga_color(r, g, b, a)
            vga_data.append(vga_color)

    # Generate C header
    header_path = f"src/{output_name}.h"
    os.makedirs("src", exist_ok=True)

    with open(header_path, 'w') as f:
        # Write header guard
        guard_name = output_name.upper() + "_H"
        f.write(f"/* {output_name}.h - VGA sprite data */\n")
        f.write(f"#ifndef {guard_name}\n")
        f.write(f"#define {guard_name}\n\n")

        # Write dimensions
        f.write(f"#define {output_name.upper()}_WIDTH {width}\n")
        f.write(f"#define {output_name.upper()}_HEIGHT {height}\n\n")

        # Write sprite data
        f.write(f"static unsigned char {output_name}[{len(vga_data)}] = {{\n")

        # Write data in rows
        for row in range(height):
            f.write("    ")
            row_start = row * width
            row_end = row_start + width
            row_data = vga_data[row_start:row_end]
            f.write(", ".join(f"{c:3d}" for c in row_data))
            if row < height - 1:
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.write(f"#endif /* {guard_name} */\n")

    print(f"  Generated {header_path} ({width}x{height}, {len(vga_data)} bytes)")
    return header_path

def convert_texture(input_path, output_name, target_width=64, target_height=64):
    """Convert texture to VGA C header file"""
    print(f"Converting texture {input_path}...")

    # Load image
    img = Image.open(input_path)

    # Convert to RGBA if not already
    if img.mode != 'RGBA':
        img = img.convert('RGBA')

    # Resize if needed (using NEAREST to preserve pixel art)
    if img.size != (target_width, target_height):
        print(f"  Resizing from {img.size} to {target_width}x{target_height}")
        img = img.resize((target_width, target_height), Image.NEAREST)

    width, height = img.size
    pixels = img.load()

    # Convert to VGA colors
    vga_data = []
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            vga_color = find_closest_vga_color(r, g, b, a)
            vga_data.append(vga_color)

    # Generate C header
    header_path = f"src/{output_name}.h"
    os.makedirs("src", exist_ok=True)

    with open(header_path, 'w') as f:
        # Write header guard
        guard_name = output_name.upper() + "_H"
        f.write(f"/* {output_name}.h - VGA texture data */\n")
        f.write(f"#ifndef {guard_name}\n")
        f.write(f"#define {guard_name}\n\n")

        # Write dimensions
        f.write(f"#define {output_name.upper()}_WIDTH {width}\n")
        f.write(f"#define {output_name.upper()}_HEIGHT {height}\n\n")

        # Write texture data
        f.write(f"static unsigned char {output_name}[{len(vga_data)}] = {{\n")

        # Write data in rows
        for row in range(height):
            f.write("    ")
            row_start = row * width
            row_end = row_start + width
            row_data = vga_data[row_start:row_end]
            f.write(", ".join(f"{c:3d}" for c in row_data))
            if row < height - 1:
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.write(f"#endif /* {guard_name} */\n")

    print(f"  Generated {header_path} ({width}x{height}, {len(vga_data)} bytes)")
    return header_path

def main():
    print("=" * 60)
    print("Converting New Sprite Set to VGA Format")
    print("=" * 60)
    print()

    # Define sprites to convert
    sprites_to_convert = [
        # Enemies - 32x32
        ("sprites_new/Creeper-1.png.png", "enemy_creeper", 32, 32),
        ("sprites_new/tomato_enemy.png", "enemy_tomato", 32, 32),
        ("sprites_new/spiderman_enemy.gif", "enemy_spiderman", 32, 32),
        ("sprites_new/waldo_enemy.gif", "enemy_waldo", 32, 32),
        ("sprites_new/snowman_enemy.png", "enemy_snowman", 32, 32),

        # Weapon - 40x40 for better visibility
        ("sprites_new/gun.png", "weapon_gun", 40, 40),
    ]

    # Define textures to convert (64x64 for tiling)
    textures_to_convert = [
        ("sprites_new/floor.png", "texture_floor", 64, 64),
        ("sprites_new/pass-through-wall.png", "texture_hologram", 64, 64),
    ]

    generated_files = []

    # Convert sprites
    print("CONVERTING ENEMY SPRITES:")
    print("-" * 60)
    for sprite_info in sprites_to_convert:
        input_path, output_name, width, height = sprite_info

        if os.path.exists(input_path):
            header_file = convert_sprite(input_path, output_name, width, height)
            generated_files.append((output_name, width, height, header_file, "sprite"))
        else:
            print(f"WARNING: {input_path} not found, skipping...")

    print()
    print("CONVERTING TEXTURES:")
    print("-" * 60)
    for texture_info in textures_to_convert:
        input_path, output_name, width, height = texture_info

        if os.path.exists(input_path):
            header_file = convert_texture(input_path, output_name, width, height)
            generated_files.append((output_name, width, height, header_file, "texture"))
        else:
            print(f"WARNING: {input_path} not found, skipping...")

    print()
    print("=" * 60)
    print("Conversion Complete!")
    print("=" * 60)
    print()
    print("Generated files:")
    for name, w, h, path, type in generated_files:
        print(f"  - {path} ({w}x{h}) [{type}]")
    print()
    print("To use in your game, add to maze_enhanced_v2.c:")
    for name, w, h, path, type in generated_files:
        print(f'  #include "{path}"')

if __name__ == "__main__":
    main()
