Using defualt buffer size with 1024 message sizes:
./server
./clien
watch -n 1 "netstat -nto | grep :8080"

Using increased buffer size with 32768 message sizes:
./server -i
./clien -i
watch -n 1 "netstat -nto | grep :8080"