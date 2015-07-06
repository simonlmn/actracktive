# Project Structure #

The projects files are organized as follows:

**`aux`** contains binary or other resources used by the application at runtime. Some of them are typically copied along with the binary output in the post-build scripts.

**`data`** contains various data files, mostly examples of processing graphs or configuration files, which are not required to build or run the application.

**`libs`** contains all third-party libraries which get **statically** linked into the application. The libraries are included as precompiled binaries for every target platform together with the required public header files.

**`src`** contains all the source code of the application.

Actracktive uses the Eclipse CDT internal builder at the moment. The Eclipse project files with all the build configurations and settings are included in the root directory. There are also some shell scripts used by the build process, namely `pre-build-osx.sh`, `pre-build-linux.sh`, `post-build-osx.sh`, `post-build-linux.sh` and `version.sh`.

After building the application, the final application binary or bundle (on OSX) will be placed inside the **`bin`** directory, which is created automatically.