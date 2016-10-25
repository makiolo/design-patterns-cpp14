#include "fooA.h"

namespace {

	foo::Base::factory::registrator<fooA::A> reg(
			foo::Base::get_factory()
		);

}

