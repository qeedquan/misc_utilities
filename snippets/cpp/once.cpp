#include <cstdio>
#include <mutex>

using namespace std;

struct OneOff
{
	OneOff()
	{
		static once_flag once;
		call_once(once, []()
		{
			printf("Should be called only once!\n");
		});
	}
};

int main()
{
	OneOff x1, x2, x3, x4;
	return 0;
}
