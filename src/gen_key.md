follow instructions for valid ssl-handshake

#generate rsa priv-key
openssl genrsa -des3 -out bitkey.key 1024

#generate cert signing request
openssl req -new -key bitkey.key -out bitkey.csr

#sing cert with priv-key
openssl x509 -req -days 3650 -in bitkey.csr -signkey bitkey.key -out bitkey.crt

#
cp bitkey.key bitkey.key.secure
openssl rsa -in bitkey.key.secure -out bitkey.key

#generate dhparam file
openssl dhparam -out dh512.pem 512

#replace the certificate and key_file params with the freshly generated and
signed key/cert-files. 

