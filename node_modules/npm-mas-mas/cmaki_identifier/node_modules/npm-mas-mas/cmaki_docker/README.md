# cmaki_docker

[![Build Status](https://travis-ci.org/makiolo/cmaki_docker.svg?branch=master)](https://travis-ci.org/makiolo/cmaki_docker)

multiple pusher of docker images.

```
for image in (windows-x86, windows-x64, linux-x86, linux-x64, ...)
  makiolo/$image = dockcross/$image + github:makiolo/cmaki_scripts/cmaki_depends.sh
done
```
