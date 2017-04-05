REM put the correct path here for the openssl binary
cd <....>\openssl-0.9.8h-1-bin\bin

echo | openssl s_client -connect script.google.com:443 | openssl x509 -fingerprint -noout
