import os
import sys


path = os.path.abspath(os.path.dirname(__file__) + "//..//..//")

def DeleteAllFiles(extension : str):
    print(f"Deleting all files with extension {extension} in {path}")

    for root, subdirs, files in os.walk(path):
        for filename in files:
            if filename.endswith(extension):
                os.remove(root + "/" + filename)

def DeleteAllFolders(name : str):
    print(f"Deleting all folders with name {name} in {path}")

    for root, subdirs, files in os.walk(path):
        for dir in subdirs:
            if dir.endswith("/" + name) or dir.endswith("\\" + name):
                os.remove(root + "/" + dir)


DeleteAllFiles(".sln")
DeleteAllFiles(".vcxproj")
DeleteAllFiles(".vcxproj.filters")
DeleteAllFiles(".vcxproj.user")