#include "fooB.h"

namespace {

	foo::Base::factory::registrator<fooB::B> reg(
			foo::Base::get_factory()
		);

}

