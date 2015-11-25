#include <assert.h>
#include "foo.h"
#include "fooA.h"
#include "fooB.h"

int main()
{
	{
		// equivalent ways of create A
		std::shared_ptr<Base> a1 = Base::factory::instance().create<A>("first parameter", 2);
		std::shared_ptr<A> a2 = Base::factory::instance().create<A>("first parameter", 2);
		std::shared_ptr<Base> a3 = Base::factory::instance().create("A", "first parameter", 2);

		// equivalent ways of create B
		std::shared_ptr<Base> b1 = Base::factory::instance().create<B>("first parameter", 2);
		std::shared_ptr<B> b2 = Base::factory::instance().create<B>("first parameter", 2);
		std::shared_ptr<Base> b3 = Base::factory::instance().create("B", "first parameter", 2);

		assert(a1 != a2);
		assert(a3 != b1);
		assert(b1 != b2);
	}

	return(0);
}

