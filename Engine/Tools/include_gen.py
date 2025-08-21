import os
import argparse

def find_header_files(directory, exts=('.h', '.hpp', '.hh')):
    header_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(exts):
                full_path = os.path.join(root, file)
                relative_path = os.path.relpath(full_path, directory).replace("\\", "/")
                header_files.append(relative_path)
    return header_files

def generate_includes(header_files):
    return [f'#include "{path}"' for path in header_files]

def main():
    parser = argparse.ArgumentParser(description="Generate #include directives for all header files in a directory.")
    parser.add_argument("directory", help="The root directory to search for headers.")
    args = parser.parse_args()

    headers = find_header_files(args.directory)
    includes = generate_includes(headers)

    for inc in includes:
        print(inc)

if __name__ == "__main__":
    main()
