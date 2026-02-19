#!/usr/bin/env python3
# Simple image to binary format converted. Used to load images from c-array inside the Engine, like the window logo.
import sys
import os
import re
from datetime import datetime

def main():
    if len(sys.argv) < 2:
        print("Usage: python _img2bin.py <image_file> [output_dir]")
        print("Output will be saved as <image_name>.c in the same directory or specified output_dir if it exists")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else os.path.dirname(input_file)
    
    # check if input file exists
    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found!")
        sys.exit(1)
    
    # read the binary data
    with open(input_file, 'rb') as f:
        data = f.read()
    
    # generate output filename
    base_name = os.path.splitext(os.path.basename(input_file))[0]
    output_file = os.path.join(output_dir, f"{base_name}.c")
    
    # create a valid C identifier from the base name
    var_name = re.sub(r'[^a-zA-Z0-9_]', '_', base_name)
    if var_name[0].isdigit():  # C identifiers can't start with a number
        var_name = '_' + var_name
    
    # write to output file
    with open(output_file, 'w') as f:
        f.write(f"/**\n")
        f.write(f" * Auto-generated from {os.path.basename(input_file)}\n")
        f.write(f" * Original size: {len(data)} bytes\n")
        f.write(f" * Generated on: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"**/\n\n")
        f.write(f"const unsigned char {var_name}_data[] = {{\n")
        
        for i in range(0, len(data), 12): # write bytes in rows of 12
            chunk = data[i:i+12]
            hex_bytes = ', '.join(f'0x{b:02x}' for b in chunk)
            f.write(f"    {hex_bytes},\n")
        
        f.write(f"}};\n\n")
        f.write(f"const unsigned long long {var_name}_size = sizeof({var_name}_data);\n")
    
    print(f"Successfully converted '{input_file}' ({len(data)} bytes)")
    print(f"Output written to: '{output_file}'")

if __name__ == "__main__":
    main()