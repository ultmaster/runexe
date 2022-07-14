#include <cstdio>
#include <cstdlib>

#include "Run.h"
#include "Interaction.h"
#include "Process.h"
#include "Strings.h"

using namespace runexe;

class Limiter {
public:
   static std::vector<InvocationResult> invoke(const InvocationParams &invocationParams); 
};

