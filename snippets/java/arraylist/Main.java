import java.util.*;

class Main {
	static class Foo {
		Foo() {
		}
	}

	public static void main(String[] args) {
		testCopy();
		testIndexOf();
	}

	public static void testCopy() {
		ArrayList<Float> flt = new ArrayList<Float>();
		flt.add(10.0f);
		flt.add(20.0f);
	
		// copy
		Float[] flo = flt.toArray(new Float[flt.size()]);
		flo[0] = 100.0f;
		flo[1] = 56.3f;
		
		for (Float f : flt)
			System.out.println(f);
		flt.set(0, flo[0]);
		flt.set(1, flo[1]);
		for (Float f : flt)
			System.out.println(f);
	}

	public static void testIndexOf() {
		Foo a = new Foo();
		Foo b = new Foo();
		Foo c = new Foo();
		ArrayList<Foo> l = new ArrayList<Foo>();
		l.add(a);
		l.add(b);
		l.add(c);
		System.out.println("Testing indexOf");
		System.out.println(l.indexOf(a));
		System.out.println(l.indexOf(b));
		System.out.println(l.indexOf(c));
	}
}
