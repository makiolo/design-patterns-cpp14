MODE ?= Debug

all: install

install:
	npm install --unsafe-perm

test: install
	npm test

run: install
	cd bin/${MODE} && LD_LIBRARY_PATH=`pwd` ./example

clean:
	-@rm -Rf bin
	-@rm -Rf artifacts
	-@rm -Rf node_modules
	-@rm -Rf gcc
	-@rm -Rf x86_64-linux-gnu-gcc
	-@rm -Rf x86_64-w64-mingw32.static-gcc
	-@rm artifacts.json

prepare:
	npm install --save-dev https://github.com/makiolo/npm-mas-mas.git

linux64:
	docker-compose run --rm linux64 make run

windows64:
	docker-compose run --rm windows64 make run

android64:
	docker-compose run --rm android64 make run


