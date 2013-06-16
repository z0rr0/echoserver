Echoserver
==========

Simple echoserver with non-block multi-client.

The ECHO service is a standard TCP/IP service: an echo server accepts connection requests from clients on some port. 

Build echoserver
-------------
    make

Build client
-------------
    make client

Additional
-------------
    #clean
    make clean
    #Copy app file in $HOME/bin:
    make install
    
Running server
-------------
    echoserver <port>
    example: ./echoserver 50000

Running client
-------------
    client <host> <port>
    example: ./client anyhost 50000
    
If chosen \<port\> is already used by some other application, server program use port from range 50000-55000.

**Tested** on Solaris 9. *Doesn't work in Linux*
