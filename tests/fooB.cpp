#include "fooB.h"

namespace fooB {
namespace regB {

	dp14::factory<foo::Base, std::string, int>::registrator<B> reg( get_factory() );

}}

