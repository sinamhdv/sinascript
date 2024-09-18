i = 0;
async {
	i = 1;
}

a = 0;
while (a < 96) {
	a = a + 1;
}

i = 2;

async{}	# a trick to join on the last async block

show(i, "\n");	# if there is a race when writing to i in the threads, this will be roughly 50-50 1 or 2


