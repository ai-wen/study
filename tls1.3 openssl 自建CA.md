start .\openssl.exe s_server -accept 443 -www -debug -msg -state -tls1_2 -CApath CApath -Verify 1
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe s_server -accept 443 -www -debug -msg -state -tls1_2 -Verify 3
openssl s_client -connect localhost:44330

openssl.exe version
openssl.exe s_server -accept 4433 -www -debug -msg -state -tls1_3 -CApath CApath -Verify 1


D:\OpenSSL-Win64_1n\bin\openssl.exe req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes
D:\OpenSSL-Win64_1n\bin\openssl.exe s_server -key key.pem -cert cert.pem -accept 44330 -www -debug -msg -state -tls1_3

D:\OpenSSL-Win64_1n\bin\openssl.exe s_client -connect localhost:44330



# nginx
查看openssl  版本
nginx -V



# [OpenSSL生成CA证书及终端用户证书](https://www.cnblogs.com/nidey/p/9041960.html)
# [Nginx配置客户端（浏览器）SSL证书认证简明教程（自签名证书）](https://devnote.pro/posts/10000002851167)
# 创建CA证书
//G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe genrsa  -out ca.key 4096
//G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe req -new -sha256 -key ca.key -out ca.csr -config ca.conf
//G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe x509 -req -days 3650 -in ca.csr -signkey ca.key -out ca.crt

G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe genrsa -des3 -out ca.key 4096
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe req -new -x509 -days 3650 -sha256 -key ca.key -out ca.crt -config ca.conf

```ini ca.conf
[ req ]
default_bits       = 4096
distinguished_name = req_distinguished_name

[ req_distinguished_name ]
countryName                 = Country Name (2 letter code)
countryName_default         = CN
stateOrProvinceName         = State or Province Name (full name)
stateOrProvinceName_default = BJ
localityName                = Locality Name (eg, city)
localityName_default        = BeiJing
organizationName            = Organization Name (eg, company)
organizationName_default    = LM
organizationalUnitName		= Organizational Unit Name (eg, section)
organizationalUnitName_default	= LM
commonName                  = Common Name (e.g. server FQDN or YOUR name)
commonName_max              = 64
commonName_default          = lm
```

# 颁发server证书
Chrome 58以后不再使用CN校验地址（就是就是浏览器地址栏URL中的那个地址host）了，而是使用SAN，注意配置里写对。
IE 11还是使用CN
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe genrsa -out server.key 2048
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe req -new -sha256 -key server.key -out server.csr -config server.conf
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe x509 -req -days 3650 -CA ca.crt -CAkey ca.key -CAcreateserial -in server.csr -out server.crt -extensions req_ext -extfile server.conf

```ini server.conf
[ req ]
default_bits       = 2048
distinguished_name = req_distinguished_name
req_extensions     = req_ext

[ req_distinguished_name ]
countryName                 = Country Name (2 letter code)
countryName_default         = CN
stateOrProvinceName         = State or Province Name (full name)
stateOrProvinceName_default = BJ
localityName                = Locality Name (eg, city)
localityName_default        = BeiJing
organizationName            = Organization Name (eg, company)
organizationName_default    = SV
organizationalUnitName		= Organizational Unit Name (eg, section)
organizationalUnitName_default	= SV
commonName                  = Common Name (e.g. server FQDN or YOUR name)
commonName_max              = 64
commonName_default          = 192.168.1.111

[ req_ext ]
subjectAltName = @alt_names

[alt_names]
#DNS.1   = www.testginx.com
IP      = 192.168.1.111
```

# client
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe genrsa -out client.key 2048
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe req -new -sha256 -key client.key -out client.csr -config client.conf
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe x509 -req -days 3650 -CA ca.crt -CAkey ca.key -CAcreateserial -in client.csr -out client.crt

验证
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe verify -verbose -CAfile ca.crt client.crt

转p12
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe pkcs12 -export -certfile ca.pem -in client.crt -inkey client.key -out client.p12
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe pkcs12 -export -clcerts -in client.crt -inkey client.key -out client.p12
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe pkcs12 -in client.p12 -out client.pem -clcerts

# 如果您为ca和客户证书使用相同的组织名称(例如，公司)，则会看到上述错误！ (编辑：重要)
```ini client.conf
[ req ]
default_bits       = 2048
distinguished_name = req_distinguished_name

[ req_distinguished_name ]
countryName                 = Country Name (2 letter code)
countryName_default         = CN
stateOrProvinceName         = State or Province Name (full name)
stateOrProvinceName_default = BJ
localityName                = Locality Name (eg, city)
localityName_default        = BeiJing
organizationName            = Organization Name (eg, company)
organizationName_default    = test
organizationalUnitName		= Organizational Unit Name (eg, section)
organizationalUnitName_default	= test
commonName                  = Common Name (e.g. server FQDN or YOUR name)
commonName_max              = 64
commonName_default          = test
```



# Nginx配置客户端（浏览器）SSL证书认证
转pem
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe x509 -in ca.crt -out ca.pem -outform PEM
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe x509 -in server.crt -out server.pem -outform PEM
G:\project\17AKCloud\4.Development\3rd\lib32\gmssl\release\bin\gmssl.exe x509 -in client.crt -out client.pem -outform PEM


server {
    listen        443;
    ssl on;
    server_name example.com;

    ssl_certificate      C:/Users/lm/Desktop/nginx-1.18.0/cer/server.crt;
    ssl_certificate_key  C:/Users/lm/Desktop/nginx-1.18.0/cer/server.key;
    ssl_client_certificate C:/Users/lm/Desktop/nginx-1.18.0/cer/ca.pem;
    ssl_verify_client on;#可以设为on，认证客户端
}


问题： ssl_client_certificate所配置证书不需要和server证书属于同一个CA根证书
server证书用于ssl服务器端认证，在生产环境一定要使用CA机构颁发的证书。
ssl_client_certificate配置的是客户端认证。

server {
        listen       443 ssl;
        server_name  192.168.1.111;

        ssl_certificate      server.crt;
        ssl_certificate_key  server.key;
	

        #ssl_session_cache    shared:SSL:1m;
        #ssl_session_timeout  5m;
	ssl_session_cache shared:le_nginx_SSL:10m;
	ssl_session_timeout 1440m;
	ssl_session_tickets off;



	ssl_client_certificate ca.pem; #一个受信任的CA证书用于验证客户端证书
	ssl_verify_client on;
	ssl_verify_depth 5;
	ssl_protocols TLSv1.3;
	
	ssl_ciphers TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES-256-GCM-SHA384:TLS13-AES-128-GCM-SHA256:EECDH+CHACHA20:EECDH+AESGCM:EECDH+AES;
    
	#ssl_ciphers "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-SHA";
	#ssl_ciphers  "EECDH+AESGCM:EDH+AESGCM:AES256+EECDH:AES256+EDH:@SECLEVEL=1";
        #ssl_ciphers  HIGH:!aNULL:!MD5;
        ssl_prefer_server_ciphers  on;	# 服务端倾向使用的加密算法
	

        location / {
            root   html;
            index  index.html index.htm;
        }
    }
