import sys
import random
import string

charset = string.ascii_letters
RANDSTR_SIZE = 6
MAP_SIZE = 1033

def do_hash(data, mod):
	p = 131
	ppow = p
	result = 0
	for c in data:
		result = (result + ppow * ord(c)) % mod
		ppow = (ppow * p) % mod
	result %= mod
	return result

def getrandstr():
	return "".join(random.choice(charset) for i in range(RANDSTR_SIZE))

def find_input(hsh, mod, count):
	res = set()
	while len(res) < count:
		while True:
			s = getrandstr()
			if do_hash(s, mod) == hsh:
				res.add(s)
				break
	return res

def main():
	out = find_input(int(sys.argv[1]), MAP_SIZE, 16)
	for s in out:
		print(s)

if __name__ == "__main__":
	main()
