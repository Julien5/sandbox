%module hamster
%include "std_string.i"
%{
#include "../statistics.h"
#include "../tickscounter.h"
%}
#define __attribute__(x)
%include "../statistics.h"
%include "../tickscounter.h"
