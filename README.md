# chttp
A simple HTTP library written for C

# dependency note(s)
this library depends upon baselib (available at
http://github.com/LeqxLeqx/baselib) which itself depents upon the pthread
libraries as well as the standard linux-C libraries

# to install
first compile the library by calling `make`,
then install by running `./install.sh`

# to link
use the gcc flags `-lchttp -lbaselib -lpthread` in that order

