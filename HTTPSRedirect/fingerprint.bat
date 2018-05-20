@echo off

REM Sujay Phadke (C) 2018

REM save cwd
pushd %cd%

REM put the correct path here for the openssl binary
cd /D "<...>"

echo | openssl s_client -connect script.google.com:443 | openssl x509 -fingerprint -noout

REM return back to the original working directory
popd
