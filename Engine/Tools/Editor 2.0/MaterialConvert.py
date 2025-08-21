import yaml
import re

# Map from old property names/indexes to new names
index_to_name = {
    0: "Albedo",
    1: "Metallic",
    2: "Roughness",
    3: "Ambient Occlusion",
    4: "Emission",
    5: "Albedo Map",
    6: "Metalloc Map",  # Note spelling, you used "Metalloc"
    7: "Roughness Map",
    8: "Normal Map",
    9: "Ambient Occlusion Map",
    10: "Emission Map",
    11: "Tilling"
}

def convert_old_material(old_text: str) -> dict:
    lines = old_text.strip().splitlines()
    output = {
        "Shader": "Default/PBR",
        "Instanced": False,
        "TransparencyMode": 0,
        "Cutout": 0,
        "Properties": {}
    }

    # Parse simple fields
    for line in lines:
        key, _, val = line.partition(":")
        key = key.strip().lower()
        val = val.strip()
        if key == "transparencymode":
            output["TransparencyMode"] = int(val)
        elif key.startswith("alpha"):
            output["Cutout"] = float(val)

    # Extract DataTypes block
    data_types_index = next((i for i, l in enumerate(lines) if "DataTypes" in l), None)
    if data_types_index is None:
        return output
    data_lines = lines[data_types_index + 1:]

    for line in data_lines:
        match = re.match(r"\s*-\s*\[(\d+),\s*\[(.*?)\],\s*(.*?),\s*(\d+)\]", line)
        if not match:
            continue

        dtype = int(match.group(1))
        value_str = match.group(2)
        prop_name = match.group(3).strip()
        index = int(match.group(4))

        key = index_to_name.get(index, f"Unknown_{index}")

        # Format values
        if dtype in (1, 2, 4):  # scalar, vec2, vec4
            values = list(map(float, value_str.split(',')))
            output["Properties"][key] = values
        elif dtype == 12:  # texture
            output["Properties"][key] = {"Default": 0}
        else:
            output["Properties"][key] = [value_str]  # fallback

    return output

# Example usage
old_material = """
Shader: 9
transparencyMode: 0
alphaCuttOff: 0.5
DataTypes:
  - [4, [1, 0, 0, 1], albedo, 0]
  - [1, 0, metallic, 1]
  - [1, 0, roughness, 2]
  - [1, 1, ao, 3]
  - [4, [1, 0.100000001, 0.100000001, 10], emission, 4]
  - [12, [0, 0, 1, white], albedoMap, 5]
  - [12, [1, 0, 1, black], metallicMap, 6]
  - [12, [2, 0, 1, black], roughnessMap, 7]
  - [12, [3, 0, 1, black], normalMap, 8]
  - [12, [4, 0, 1, black], aoMap, 9]
  - [12, [5, 0, 1, white], emissionMap, 10]
  - [2, [1, 1], textureTilling, 11]
"""


fileName = input("Input file: ")
with open(fileName, "r", encoding="utf-8") as file:
    old_material = file.read()

converted = convert_old_material(old_material)

# Write output to a new file
outFile = fileName + ".smaterial"
with open(outFile, "w", encoding="utf-8") as file:
    file.write(yaml.dump(converted, sort_keys=False))
