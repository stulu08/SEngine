import re

def replace_uuids(scene_text):
    gameobject_pattern = re.compile(r'(?<=GameObject: )\d+')
    parent_pattern = re.compile(r'(?<=parent: )\d+')
    
    # Find all GameObject UUIDs
    uuids = gameobject_pattern.findall(scene_text)
    
    # Create a mapping from old UUIDs to new sequential numbers
    uuid_mapping = {old: str(i) for i, old in enumerate(uuids)}
    
    # Replace GameObject UUIDs
    scene_text = gameobject_pattern.sub(lambda m: uuid_mapping[m.group()], scene_text)
    
    # Replace Parent UUIDs
    scene_text = parent_pattern.sub(lambda m: uuid_mapping.get(m.group(), m.group()), scene_text)
    
    return scene_text

# Read input scene file
fileName = input("Input file: ")

with open(fileName, "r", encoding="utf-8") as file:
    scene_data = file.read()

# Process the scene file
cleaned_scene = replace_uuids(scene_data)

# Write output to a new file
outFile = "cleaned_" + fileName

with open(outFile, "w", encoding="utf-8") as file:
    file.write(cleaned_scene)

print("Scene file cleaned and saved as " + outFile)
