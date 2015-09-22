// annotations python style in C++
// author: Ricardo Marmolejo Garcia
#include <iostream>
#include <functional>
#include <string>
#include <sstream>

// http://blog.sigfpe.com/2006/08/you-could-have-invented-monads-and.html
// http://stackoverflow.com/questions/44965/what-is-a-monad
// http://www.cplusplus.com/doc/tutorial/operators/
// https://github.com/mattn/emmet-vim

using leaf = std::function<std::string()>;
using composite = std::function<leaf(const leaf&)>;

leaf operator+=(const composite& a, const leaf& b)
{
	return a(b);
}

leaf operator,(const leaf& a, const leaf& b)
{
	return [&]()
	{
		return a() + b();
	};
}

leaf operator*(int n, const leaf& f)
{
	return [&]()
	{
	    std::stringstream ss;
	    for(int i=0; i<n; ++i)
	        ss << f();
	    return ss.str();
	};
}

composite operator*(int n, const composite& a)
{
	return [&](const leaf& f)
	{
		return [&]()
		{
		    std::stringstream ss;
		    for(int i=0; i<n; ++i)
		        ss << a(f)();
		    return ss.str();
		};
	};
}

composite tag(const decltype(leaf()())& tag)
{
	return [=](const leaf& f)
	{
		return [&]() {return "<" + tag + ">\n" + f() + "</" + tag + ">\n";};
	};
}

leaf content(const decltype(leaf()())& a)
{
	return [&]() { return a; };
}

int main(int argc, const char *argv[])
{
    std::cout << (
                    tag("html") += 
                    (
                            tag("head") += 
                            (
                                    tag("title") += content("title page")
                                    ,
                                    tag("script") += content("alert(a);")
                                    ,
                                    tag("script") += content("alert(b);")
                                    ,
                                    tag("script") += content("alert(c);")
                            )
                            ,
                            tag("body") += 
                            (
                                tag("h1") += content("Hello world!")
                                ,
                                tag("ul") += 5*tag("li") += 2*content("element $")
                            )
                    )
                    )() << std::endl;
	return 0;
}

