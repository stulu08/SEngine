import os
import sys
import shutil

delimeter = "/"

if os.name == "nt":
    delimeter = "\\"

path = os.path.abspath(os.path.dirname(__file__) + "//..//..//")

def DeleteAllFiles(extension : str):
    for root, subdirs, files in os.walk(path):
        for filename in files:
            if filename.endswith(extension):
                file = root + delimeter + filename
                print(f"--> {file}")
                os.remove(file)

def DeleteAllFolders(name : str):
    for root, subdirs, files in os.walk(path):
        for dir in subdirs:
            if dir.endswith(name):
                folder = root + delimeter + dir
                print(f"--> {folder}")
                shutil.rmtree(folder)



print(f"Deleting all project file in {path}")
DeleteAllFiles(".sln")
DeleteAllFiles(".vcxproj")
DeleteAllFiles(".vcxproj.filters")
DeleteAllFiles(".vcxproj.user")
print("\n\n")

print(f"Deleting all bin/bin-int folders in {path}")
DeleteAllFolders("bin")
DeleteAllFolders("bin-int")

buildDir = path + delimeter + "Engine" + delimeter + "Build"
if os.path.exists(buildDir):
    print("\n\n")
    print(f"Deleting build folder {buildDir}")
    shutil.rmtree(buildDir)

