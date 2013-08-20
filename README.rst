Mapache webserver
=================

Mapache is a webserver developed during the Network Programming course MAC5910 taken at IME/USP in 2013.

.. image:: https://api.travis-ci.org/mscs-usp/mac5910-webserver.png?branch=master
   :target: https://api.travis-ci.org/mscs-usp/mac5910-webserver

Teste...

Development debug
-----------------

Debuging with curl:

.. code-block:: bash

    curl -v -X OPTIONS http://<IP>

Output:

.. code-block:: bash

    * About to connect() to <IP> port 80 (#0)
    *   Trying <IP>...
    * connected
    * Connected to <IP> (<IP>) port 80 (#0)
    > OPTIONS / HTTP/1.1
    > User-Agent: curl/7.28.1
    > Host: <IP>
    > Accept: */*
    > 
    < HTTP/1.1 200 OK
    < Date: Tue, 20 Aug 2013 22:47:55 GMT
    < Server: Apache/2.2.22 (Linux/SUSE)
    < Allow: POST,OPTIONS,GET,HEAD,TRACE
    < Content-Length: 0
    < Content-Type: text/html
    < 
    * Connection #0 to host <IP> left intact
    * Closing connection #0
