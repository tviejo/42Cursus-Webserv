GET http://localhost:8080/www

HTTP/1.1 200
[Asserts]
header "Content-Type" == "text/html"

