#include "foo.h"

namespace foo {

Base::factory& get_factory()
{
	return Base::factory::instance();
}

}

