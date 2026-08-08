class Bar extends Foo {
	Bar() {
		System.out.printf("Bar count %d\n", getCount());
	}

	int getCount() {
		return 5;
	}
};
