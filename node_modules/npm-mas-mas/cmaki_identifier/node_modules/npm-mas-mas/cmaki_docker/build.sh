#!/usr/bin/env bash
#/bin/bash
prefix=$(pwd)/bin
mkdir -p $prefix

# iterate in known images
curl https://raw.githubusercontent.com/dockcross/dockcross/master/Makefile -o dockcross-Makefile
for image in $(make -f dockcross-Makefile display_images); do
	if [[ $(docker images -q dockcross/$image) != "" ]]; then
		docker rmi -f dockcross/$image
		echo dockcross/$image removed.
	fi
done

for image in $(make -f dockcross-Makefile display_images); do

	if [[ "$image" == "manylinux-x86" ]]; then
		continue
	fi

	if [[ "$image" == "manylinux-x64" ]]; then
		continue
	fi

	echo "copy dockcross/$image to makiolo/$image (with script change)"
	cat<<EOF>Dockerfile
FROM dockcross/$image:latest
ENV DEBIAN_FRONTEND noninteractive
RUN curl -s https://raw.githubusercontent.com/makiolo/cmaki_scripts/master/cmaki_depends.sh | bash
EOF

	docker login -u $DOCKER_USER -p $DOCKER_PASSWORD
	docker build . -t makiolo/$image
	docker push makiolo/$image

	# clean
	docker rmi -f dockcross/$image
	docker rmi -f makiolo/$image
done

