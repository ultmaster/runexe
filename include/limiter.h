#include <cstdio>
#include <cstdlib>

#include "run.h"
#include "interaction.h"
#include "process.h"
#include "strings.h"

using namespace runexe;

class Limiter {
public:
   static std::vector<InvocationResult> invoke(const InvocationParams &invocationParams); 
};

