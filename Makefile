MODE ?= Debug

all: clean prepare install test

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

install:
	npm install --unsafe-perm

test: install
	npm test

linux64:
	docker-compose run --rm -e MODE=${MODE} linux64 make

windows64:
	docker-compose run --rm -e MODE=${MODE} windows64 make

android64:
	docker-compose run --rm -e MODE=${MODE} android64 make

