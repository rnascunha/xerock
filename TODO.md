#TODO List

##Build
 
* Make all libs at `src/libs/` external libraries
 
* Make it installable (?)
 
* Make it as a service (security issues about??)
 
* Option to embedded SSL key/certificate at binary. 
 
##Geneal

* Add log

* Open a command line (like console) to send commands at run time (open/close/list resources)

###Command line

* Read config from file

* Create config file from command line (instead of run app, just print "config file")  

##Apps

* Create version of apps

* Externalize?

* Enable/disable apps at run time

* New apps ideas: ~TCP Client~, UDP Server/Client, CoAP Server/Client...

###Serial

* Configure other serial options: signals, flowcontrol...

###Monitor

* Port to Windows (?)

###TCP Server

* Send keepalive config at status message (show at interface)

* Set other socket options (SO_LINGER...)

##Error

* 2 messages been sent when some user disconnect from daemon;
