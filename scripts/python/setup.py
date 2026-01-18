import os
import sys
import subprocess
import platform

from setupPython import PythonConfiguration as PythonRequirements

# Make sure everything we need for the setup is installed
PythonRequirements.Validate()

from setupPremake import PremakeConfiguration as PremakeRequirements
from setupVulkan import VulkanConfiguration as VulkanRequirements
from setupMono import MonoConfiguration as MonoRequirements
os.chdir('./../') # Change from scripts/python directory to root

premakeInstalled = PremakeRequirements.Validate()
VulkanRequirements.Validate()
MonoRequirements.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if (len(sys.argv) > 1 and sys.argv[1] == "nobuild"):
    sys.exit(0)

if (premakeInstalled):
    #if platform.system() == "Windows":
    #    print("\nRunning premake...")
    #    subprocess.call([os.path.abspath("./scripts/Win-GenProjects.bat"), "nopause"])
    print('\nSelect action')
    print("   0: Visual Studio 2022")
    print("   1: Visual Studio 2026")
    print("   2: GNU Makefiles")
    inp = int(input())
    if (inp == 0):
        projectGenPath = os.path.abspath("scripts/generateProjects - vs2022.bat")
        print("\nRunning " + projectGenPath)
        subprocess.call([projectGenPath, "nopause"])
    elif (inp == 1):
        projectGenPath = os.path.abspath("scripts/generateProjects - vs2026.bat")
        print("\nRunning " + projectGenPath)
        subprocess.call([projectGenPath, "nopause"])
    elif (inp == 2):
        projectGenPath = os.path.abspath("scripts/generateProjects - makefile.bat")
        print("\nRunning " + projectGenPath)
        subprocess.call([projectGenPath, "nopause"])
    else:
        print("Could not identify project file type, skipping step")
    print("Setup completed!")
else:
    print("Stulu requires Premake to generate project files.")

