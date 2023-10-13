import os
import sys
import subprocess
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen


class MonoConfiguration:
    #https://download.mono-project.com/archive/6.12.0/windows-installer/mono-6.12.0.182-x64-0.msi
    requiredMonoVersion = "6.12.0"
    latestBuild = "182"
    monoInstallerDirectory = "./Stulu/vendor/Mono"
    defaultMonoDir = "C:/Program Files/Mono"
    
    @classmethod
    def Validate(cls):
        if (not cls.CheckMono()):
            print("Mono not installed correctly.")
            return

    @classmethod
    def CheckMono(cls):
        mono = os.environ.get("MONO_PATH")
        if (mono is None):
            print("Checking for mono at path " + cls.defaultMonoDir)
            
            if(os.path.isdir(cls.defaultMonoDir)):
                mono = cls.defaultMonoDir
            else:
                cls.__InstalllMono()
                return False
        else:
            print(f"\nLocated Mono at {mono}")
        
        
        monoVersion = subprocess.check_output([mono+"/bin/mono", '--version']).decode(sys.stdout.encoding)
        
        if (cls.requiredMonoVersion not in monoVersion):
            print(f"You don't have the correct Mono version! (Engine requires {cls.requiredMonoVersion})")
            cls.__InstalllMono()
            return False
    
        print(f"Correct Mono located at {mono}")
        if(os.environ.get("MONO_PATH") is None):
            #set MONO_PATH
            print("Trying to set environment MONO_PATH to " + cls.defaultMonoDir)
            os.system(f"setx \"MONO_PATH\" \"{cls.defaultMonoDir}\" /m")
            print(f"Environment variable 'MONO_PATH' was automaticly set")
            print("\nRe-run this script to update Environment variables")
            quit()
        return True

    @classmethod
    def __InstalllMono(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Would you like to install Mono {0:s}? [Y/N]: ".format(cls.requiredMonoVersion))).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        monoInstallURL = f"https://download.mono-project.com/archive/{cls.requiredMonoVersion}/windows-installer/mono-{cls.requiredMonoVersion}.{cls.latestBuild}-x64-0.msi"
        monoPath = f"{cls.monoInstallerDirectory}/mono-{cls.requiredMonoVersion}.{cls.latestBuild}-x64-0.msi"
        print("Downloading {0:s} to {1:s}".format(monoInstallURL, monoPath))
        Utils.DownloadFile(monoInstallURL, monoPath)
        print("Running Mono installer...")
        os.startfile(os.path.abspath(monoPath))
        
        #set MONO_PATH
        print("Trying to set environment MONO_PATH to " + cls.defaultMonoDir)
        os.system(f"setx \"MONO_PATH\" \"{cls.defaultMonoDir}\" /m")
        print(f"Environment variable 'MONO_PATH' was automaticly set to: {os.environ.get('MONO_PATH')}")
        
        print("Re-run this script after installation!")
        quit()

if __name__ == "__main__":
    MonoConfiguration.Validate()