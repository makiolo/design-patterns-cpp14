PACKAGE ?= .
MODE ?= Debug

all: clean build

build:
	(cd cmaki_identifier && npm install --unsafe-perm)
	(cd cmaki_generator && ./build ${PACKAGE} -d)

clean:
	(cd cmaki_identifier && rm -Rf bin rm -Rf artifacts)

linux64:
	docker-compose run --rm -e PACKAGE=${PACKAGE} -e MODE=${MODE} linux64 make

windows64:
	docker-compose run --rm -e PACKAGE=${PACKAGE} -e MODE=${MODE} windows64 make

android64:
	docker-compose run --rm -e PACKAGE=${PACKAGE} -e MODE=${MODE} android64 make

