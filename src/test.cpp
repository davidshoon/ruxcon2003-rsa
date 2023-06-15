/*
	Ruxcon 2003 "Attacking RSA Key generators" by icebsd - revisited

	2022/03/05
		Factorising the 779-bit key using pollard p-1.
		Also factorises the 268-bit key using pollard p-1.
		Optimised code so that it evaluates the modular exponentiation much faster.

	2022/03/07
		Added the 127-bit key, and changed "max" to 2M so that it can factorise the 127-bit key.
		Also added bit/digit size indicator.

	To compile: g++ test.cpp -lgmpxx -lgmp

	You will need to "sudo apt install libgmp-dev" (ubuntu).

	Author: David Shoon

	Notes: I guess it's almost 20 years onwards since the actual presentation,
	and I had to pull my own presentation slides from archive.org. LOL. I've since
	lost all source code which I used for the presentation, including a weak
	primes key generator, which was used to generate this 779-bit key.

	This is for those who wanted to find out the prime numbers to the various
	keys in the presentation, but don't know how to code in C/C++.

	I hope it's of some use / knowledge sharing.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <iostream>

#include <math.h>
#include <gmp.h>
#include <gmpxx.h>

mpz_class pollard_p_minus_1(mpz_class c, mpz_class max, mpz_class n)
{
	mpz_class m = c;
	mpz_class g;

	for (mpz_class i = 1; i < max; i++) {
//		mpz_class res;
//		mpz_pow_ui(res.get_mpz_t(), m.get_mpz_t(), mpz_get_ui(i.get_mpz_t()) );
//		m = res % n;

// faster version:
		mpz_powm(m.get_mpz_t(), m.get_mpz_t(), i.get_mpz_t(), n.get_mpz_t());

		if (i % 10 == 0) {
			mpz_class temp = m - 1;
			mpz_gcd(g.get_mpz_t(), temp.get_mpz_t(), n.get_mpz_t());
			if (g > 1) {
				if ((n % g == 0) && (g != 1) && (g != n)) {
					std::cout << "g: " << g << std::endl;
					return g;
				}
			}
		}
	}

	return -1;
}

void func(mpz_class a)
{
	std::cout << "a: " << a << " :" << mpz_sizeinbase(a.get_mpz_t(), 2) << " bits " << mpz_sizeinbase(a.get_mpz_t(), 10) << " digits" << std::endl;

	mpz_class g;

	for (int i = 2; i < 300; i++) {
		std::cout << "i: " << i << std::endl;
		g = pollard_p_minus_1(i, 2000000, a);
		if (g != -1) {
			break;
		}
	}

	if (mpz_probab_prime_p(g.get_mpz_t(), 50)) {
		std::cout << "found prime!" << std::endl;
	}

	std::cout << "prime: " << g << std::endl;

	mpz_class h = a / g;

	if (mpz_probab_prime_p(h.get_mpz_t(), 50)) {
		std::cout << "found prime!" << std::endl;
	}

	std::cout << "other prime: " << h << std::endl;
}

int main(int argc, char **argv)
{
#if 0

// attack the default keys in the Ruxcon 2003 presentation...

//	func(2967712933176139505325218827281016830172885377668890665422303521091925853065808657763875660509418806842054224785177504767070016206910781672103368981812004063609160919203696870514423547321725245287923332418660222579507200000000000000001_mpz); // 779-bit key
//	func(253492469520653197083351904322634118562257623727339245721183851919106703858280081_mpz); // 268-bit key
//	func(154048638727715990908093481862028012019_mpz); // 127-bit key
#endif

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <number to factorise>\n", argv[0]);

		fprintf(stderr, "\te.g.\n");

		fprintf(stderr, "\t%s 2967712933176139505325218827281016830172885377668890665422303521091925853065808657763875660509418806842054224785177504767070016206910781672103368981812004063609160919203696870514423547321725245287923332418660222579507200000000000000001 -- 779-bit key\n", argv[0]); // 779-bit key
		fprintf(stderr, "\t%s 253492469520653197083351904322634118562257623727339245721183851919106703858280081 -- 268-bit key\n", argv[0]); // 268-bit key
		fprintf(stderr, "\t%s 154048638727715990908093481862028012019 -- 127-bit key\n", argv[0]); // 127-bit key

		exit(1);
	}

	const char *number = argv[1];

	func(mpz_class(number, 10)); // factorise the number, expect it in base 10...

	return 0;
}
