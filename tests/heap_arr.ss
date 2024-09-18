i = 0;
a = [1,2,3];
while (i < 2) {
	if (i == 0) {
		b = [a];
	} else {
		a = [b];
	}
	i = i + 1;
}
show(a, "\n");
show(b, "\n");

#free the objects
a = 5;
b = 5;

