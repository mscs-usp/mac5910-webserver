echo "DUMPING SERVER ON PORT: $1 TO FILE $2"

echo "START OF DUMP" > $2
echo "---" >> $2
echo "POST FOUND" >> $2
echo "---" >> $2
curl -v -is --request POST --data "param1=value1&param2=value2" http://127.0.0.1:$1/index.html | tee >> $2
echo "---" >> $2
echo "GET FOUND" >> $2
echo "---" >> $2
curl -v -is --request GET http://127.0.0.1:$1/index.html | tee >> $2
echo "---" >> $2
echo "OPTIONS" >> $2
echo "---" >> $2
curl -v -is --request OPTIONS http://127.0.0.1:$1/index.html | tee >> $2

echo "---" >> $2
echo "POST NOT FOUND" >> $2
echo "---" >> $2
curl -v -is --request POST --data "param1=value1&param2=value2" http://127.0.0.1:$1/notfound.html | tee >> $2
echo "---" >> $2
echo "GET NOT FOUND" >> $2
echo "---" >> $2
curl -v -is --request GET http://127.0.0.1:$1/notfound.html | tee >> $2
