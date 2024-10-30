GET http://localhost:8080/nonexistent

HTTP/1.1 404
[Asserts]
header "Content-Type" == "text/html"

