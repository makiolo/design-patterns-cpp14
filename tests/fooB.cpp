#include "fooB.h"

// register implementations to static factory
namespace regB
{
	Base::factory::registrator<B> reg;
}

