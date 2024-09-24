# implementation of insertion sort in SinaScript

arr = [48, -19, 34, 12, 53, 0, 1, -99, 28, 28, 8, 12];

show("The array before sorting:", arr, "\n");

i = 1;
while (i < length(arr)) {
	j = i;
	while (j > 0) {
		if (arr[j - 1] > arr[j]) {
			tmp = arr[j - 1];
			arr[j - 1] = arr[j];
			arr[j] = tmp;
		}
		j = j - 1;
	}
	i = i + 1;
}

show("The sorted array is:", arr, "\n");
