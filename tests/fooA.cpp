#include "fooA.h"

// register implementations to static factory
namespace regA
{
	Base::factory::registrator<A> reg;
}

