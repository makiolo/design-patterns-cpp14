#include "foo.h"

foo::Base::factory& foo::Base::get_factory()
{
	return foo::Base::factory::instance();
}
