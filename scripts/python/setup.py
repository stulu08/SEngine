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
    print("   \"-vs22\" for Visual Studio 2022")
    print("   \"-vs19\" for Visual Studio 2019")
    print("   \"-make\" for GNU Makefiles")
    inp = input()
    if (inp == "-vs22" or inp == "vs22" or inp == "-vs2022" or inp == "vs2022"):
        projectGenPath = os.path.abspath("scripts/generateProjects - vs2022.bat")
        print("\nRunning " + projectGenPath)
        subprocess.call([projectGenPath, "nopause"])
    elif (inp == "-vs19" or inp == "vs19" or inp == "-vs2019" or inp == "vs2019"):
        projectGenPath = os.path.abspath("scripts/generateProjects - vs2019.bat")
        print("\nRunning " + projectGenPath)
        subprocess.call([projectGenPath, "nopause"])
    elif (inp == "-make" or inp == "make" or inp == "-makefile" or inp == "makefile"):
        projectGenPath = os.path.abspath("scripts/generateProjects - makefile.bat")
        print("\nRunning " + projectGenPath)
        subprocess.call([projectGenPath, "nopause"])
    else:
        print("Could not identify project file type")
    print("Setup completed!")
else:
    print("Stulu requires Premake to generate project files.")

