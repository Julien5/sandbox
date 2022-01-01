%module hamster
%include "std_string.i"
%{
#include "tickscounter.h"
%}
#define __attribute__(x)
%include "tickscounter.h"
