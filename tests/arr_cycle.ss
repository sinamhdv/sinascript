a = [5, 6];
a[0] = a;
a[0] = 7;
a[0] = a;
ref = a[0];
ref[0] = 3;
show("a is:", a, "\n");

i=0;
