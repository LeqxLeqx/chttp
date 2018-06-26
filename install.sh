#!/bin/bash


if [[ $UID != 0 ]] ; then
  echo must run as root
  exit 1
fi

if [[ !(-e bin/libchttp.so) ]] ; then
  echo cannot find a file at bin/libchttp.so
  exit 2
fi 

if [[ -d /usr/include/chttp ]] ; then
  rm -r /usr/include/chttp
  if [[ $? != 0 ]] ; then
    echo failed to remove existing header file directory at /usr/include/chttp
    exit 2
  fi
fi


cp bin/libchttp.so /usr/lib/libchttp.so
if [[ $? != 0 ]];  then
  echo failed to copy shared object file to /usr/lib/libchttp.so
  exit 4
fi

mkdir /usr/include/chttp
if [[ $? != 0 ]] ; then
  echo failed to create directory at /usr/include/chttp
  exit 4
fi

cp -r src/*.h /usr/include/chttp/
if [[ $? != 0 ]] ; then
  echo failed to copy header files to directory /usr/include/chttp/
  exit 4
fi

exit 0

