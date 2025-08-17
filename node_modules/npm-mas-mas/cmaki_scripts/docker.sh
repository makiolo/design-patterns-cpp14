#!/bin/bash
export IMAGE="${IMAGE:-linux-x64}"
export MODE="${MODE:-Debug}"
export NPP_CACHE="${NPP_CACHE:-FALSE}"
export PACKAGE="${PACKAGE:-undefined}"

docker run --rm makiolo/$IMAGE > ./dockcross-$IMAGE
sed -e "s#DEFAULT_DOCKCROSS_IMAGE=dockcross/$IMAGE#DEFAULT_DOCKCROSS_IMAGE=makiolo/$IMAGE#g" dockcross-$IMAGE > makiolo-$IMAGE
chmod +x ./makiolo-$IMAGE
if [ "$PACKAGE" == "undefined" ]; then
	# CI
	./makiolo-$IMAGE -a "-e MODE=$MODE -e NPP_CACHE=$NPP_CACHE -e DEFAULT_DOCKCROSS_IMAGE=makiolo/$IMAGE" bash -c 'curl -s https://raw.githubusercontent.com/makiolo/cmaki_scripts/master/ci.sh | bash'
else
	# build package
	./makiolo-$IMAGE -a "-e MODE=$MODE -e NPP_CACHE=$NPP_CACHE -e DEFAULT_DOCKCROSS_IMAGE=makiolo/$IMAGE -e PACKAGE=$PACKAGE" bash -c 'curl -s https://raw.githubusercontent.com/makiolo/cmaki_scripts/master/make_artifact.sh | CMAKI_INSTALL=$(pwd)/bin bash'
fi
error=$?

# clean container
docker rmi -f makiolo/$IMAGE

exit $error
