#!/bin/bash


step_qmake()
{
	mkdir build
	cd build
	qmake ../modbus-watcher.pro 
	cd ..
  	return 0
}

step_make()
{
  cd build
  mingw32-make
  return 0
}

step_package()
{
  echo "Package"
  return 0
}

print_failed_step() {
	echo -e "\e[31m$1 failed\e[0m" >&2
}

case "$1" in
  "qmake")
    step_qmake;
    ;;
  "make")
	step_make;
    ;;
  "package")
	step_package;
    ;;
  *)
	echo "╔═══════════════════╗"
    echo "║ Running all steps ║"
    echo "╚═══════════════════╝"
    echo -e "\e[31mRUNNINGIN RED\e[0m"
    sleep 5
    echo -e "\e[2A"
    echo -e "\e[32mRUNNINGIN GREEN\e[0m"


	step_qmake

	if [[ $? -eq 1 ]]; then
	    print_failed_step "qmake";
	fi

    step_make

	if [[ $? -eq 1 ]]; then
	    echo "make failed" >&2
	fi

    step_package

	if [[ $? -eq 1 ]]; then
	    echo "\"package\" failed" >&2
	fi

    exit 0
    ;;
esac
