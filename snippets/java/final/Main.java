class Main {
	final class Bar {
		final class Foo {
			static final int BAZ = 12;
		}
		static final String BOR = "B0RK";
	}

	public static void main(String[] args) {
		System.out.println(Bar.Foo.BAZ);
		System.out.println(Bar.BOR);
	}
}
