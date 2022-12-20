import os;
import sys;
import platform;
import subprocess;
import shutil;
import requests;
import tqdm;

class Util :

    @classmethod
    def Download(cls, url, path) :

        if(os.path.isfile(path) is True) : return;

        dlrequest = requests.get(url, stream = True);
        dlpercentage = int(dlrequest.headers.get("content-lenght", 0));

        with open(path, "wb") as file, tqdm.tqdm(
            desc = path, total = dlpercentage, unit = "iB", unit_scale = True, unit_divisor = 1024,
        ) as bar :
            for data in dlrequest.iter_content(chunk_size = 1024) :
                size = file.write(data);
                bar.update();
    
    @classmethod
    def Decompress(cls, file, path) :

        if(os.path.isfile(file) is None) : return;
        else : shutil.unpack_archive(file, path);

class Premake :

    version = "5.0.0-beta2";
    path = "Thirdparty/premake5";

    @classmethod
    def Download(cls) :

        if(os.path.isdir(f"{cls.path}") is False) : os.makedirs(f"{cls.path}");

        url = "";
        file = "";

        if(platform.system() == "Windows") :
            url = f"https://github.com/premake/premake-core/releases/download/v{cls.version}/premake-{cls.version}-windows.zip";
            file = f"{cls.path}/premake-{cls.version}-windows.zip";
        
        if(platform.system() == "Linux") :
            url = f"https://github.com/premake/premake-core/releases/download/v{cls.version}/premake-{cls.version}-linux.tar.gz";
            file = f"{cls.path}/premake-{cls.version}-linux.tar.gz";
        
        if(platform.system() == "Darwin") :
            url = f"https://github.com/premake/premake-core/releases/download/v{cls.version}/premake-{cls.version}-macosx.tar.gz";
            file = f"{cls.path}/premake-{cls.version}-macosx.tar.gz";

        Util.Download(url, file);
        Util.Decompress(file, cls.path);

    @classmethod
    def Generate(cls) :

        if(len(sys.argv) == 2) :
            param_1= sys.argv[1];
            subprocess.call(f"{cls.path}/premake5 {param_1}");

        else :
            if(platform.system() == "Windows") : subprocess.call(f"{cls.path}/premake5 vs2022");
            if(platform.system() == "Linux") : subprocess.call(f"{cls.path}/premake5 gmake2");
            if(platform.system() == "Darwin") : subprocess.call(f"{cls.path}/premake5 xcode4");

Premake.Download();
Premake.Generate();