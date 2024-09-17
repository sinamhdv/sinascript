a = 10;
b = 15;
arr = [1, 2, a];
i = 0;
while (i < 2) {
	a = (a + 2 * 19 / 4 - 2) - b / 7 - arr[0];
	arr[0] = a * 2;
	i = i + 1;
}
show(a, b, i);
show(arr);
show("hello" + " " + "world!");