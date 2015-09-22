// annotations python style in C++
// author: Ricardo Marmolejo Garcia
#include <iostream>
#include <functional>
#include <string>
#include <sstream>

// http://blog.sigfpe.com/2006/08/you-could-have-invented-monads-and.html
// http://stackoverflow.com/questions/44965/what-is-a-monad

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

composite repeat(int n)
{
	return [=](const leaf& f)
	{
		return [&]()
		{
			std::stringstream ss;
			for(int i=0; i<n; ++i)
			{
				ss << f();
			}
			return ss.str();
		};
	};
}

composite tag(const std::string& tag)
{
	return [=](const leaf& f)
	{
		return [&]() {return "<" + tag + ">" + f() + "</" + tag + ">";};
	};
}

leaf content(const std::string& a)
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
                                tag("ul") += repeat(5) += tag("li") += content("element")
                            )
                    )
                    )() << std::endl;
    /*
    Generate:
    <html>
    <head>
        <title>title page</title>
        <script>alert(a);</script>
        <script>alert(b);</script>
        <script>alert(c);</script>
    </head>
    <body>
    <h1>Hello world!</h1>
    <ul>
        <li>element</li>
        <li>element</li>
        <li>element</li>
        <li>element</li>
        <li>element</li>
    </ul>
    </body>
    </html>
    */
	return 0;
}

