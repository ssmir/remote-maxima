Maxima Service can be used after remote-maxima has been built using BuildInstructions.

## Deployment ##

Deployment process involves the following files:
  1. bin/node - The IARnet container, that is a server that can configured to provide remote access to the Maxima Service.
  1. lib/libIARnet.so - A shared library containing code shared between services and the container.
  1. lib/libMaximaService.so - A shared library containing the Maxima Service.
  1. conf/node.conf - A sample configuration file for the IARnet container.

TDC

## Usage (writing a client) ##
TBD