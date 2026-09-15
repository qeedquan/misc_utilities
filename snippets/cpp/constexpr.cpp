#include <cstdio>

struct element
{
	int spacing;
	int scale;
	int fem;
};

auto elem = []() constexpr -> element
{
	int spacing = 20;
	for (int i = 0; i < 10; i++)
		spacing += i;
	int scale = spacing / 2;
	int fem = 30 / spacing + 1234;

	return element{spacing, scale, fem};
}();

int main()
{
	printf("%d %d %d\n", elem.spacing, elem.scale, elem.fem);
	return 0;
}
