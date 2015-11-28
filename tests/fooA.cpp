#include "fooA.h"

namespace fooA {
namespace regA {

	dp14::factory<foo::Base, std::string, int>::registrator<A> reg( get_factory() );

}}

