import yaml
import os
from pathlib import Path

# Mapping from old integer type to new type names
type_map = {
    0: "MiscFileAsset",
    1: "Texture2DAsset",
    2: "RenderTextureAsset",
    3: "SkyBoxAsset",
    4: "ModelAsset",
    5: "MeshAsset",
    6: "MaterialAsset",
    7: "ShaderAsset",
    8: "SceneAsset",
    9: None,  # Directory -> Delete (skip file)
    10: "ScriptAsset"
}

def convert_meta_file(input_path: Path, output_path: Path):
    with open(input_path, "r", encoding="utf-8") as f:
        data = yaml.safe_load(f)

    # Skip if no type or if it's a directory
    type_id = data.get("type", None)
    uuid = data.get("uuid", None)
    
    if type_id is None or uuid is None:
        print(f"[SKIPPED] Invalid file: {input_path}")
        return
    
    new_type = type_map.get(type_id, "MiscFileAsset")
    if new_type is None:
        print(f"[DELETED] {input_path} (was type Directory)")
        return  # Skip writing/deleting Directory types

    # Build new structure
    new_data = {
        "UUID": uuid,
        "TypeName": new_type
    }

    # Preserve other fields
    for k, v in data.items():
        if k not in ("uuid", "type"):
            new_data[k] = v

    # Write new format
    with open(output_path, "w", encoding="utf-8") as f:
        yaml.dump(new_data, f, sort_keys=False)

    print(f"[CONVERTED] {input_path} -> {output_path}")

def batch_convert_meta_folder(folder: str):
    folder_path = Path(folder)
    for meta_file in folder_path.rglob("*.meta"):
        convert_meta_file(meta_file, meta_file)

# --- Example usage ---
# Set this to your .meta folder
# batch_convert_meta_folder("path/to/your/assets")
