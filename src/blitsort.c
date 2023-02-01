/*
	Copyright (C) 2014-2022 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	blitsort 1.1.5.4
*/

#define BLIT_AUX 512 // set to 0 for sqrt(n) cache size
#define BLIT_OUT  24

void FUNC(blit_partition)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp);

void FUNC(blit_analyze)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	unsigned char loop, asum, bsum, csum, dsum;
	unsigned int astreaks, bstreaks, cstreaks, dstreaks;
	size_t quad1, quad2, quad3, quad4, half1, half2;
	size_t cnt, abalance, bbalance, cbalance, dbalance;
	VAR *pta, *ptb, *ptc, *ptd;

	half1 = nmemb / 2;
	quad1 = half1 / 2;
	quad2 = half1 - quad1;
	half2 = nmemb - half1;
	quad3 = half2 / 2;
	quad4 = half2 - quad3;

	pta = array;
	ptb = array + quad1;
	ptc = array + half1;
	ptd = array + half1 + quad3;

	astreaks = bstreaks = cstreaks = dstreaks = 0;
	abalance = bbalance = cbalance = dbalance = 0;

	for (cnt = nmemb ; cnt > 132 ; cnt -= 128)
	{
		for (asum = bsum = csum = dsum = 0, loop = 32 ; loop ; loop--)
		{
			asum += cmp(pta, pta + 1) > 0; pta++;
			bsum += cmp(ptb, ptb + 1) > 0; ptb++;
			csum += cmp(ptc, ptc + 1) > 0; ptc++;
			dsum += cmp(ptd, ptd + 1) > 0; ptd++;
		}
		abalance += asum; astreaks += asum = (asum == 0) | (asum == 32);
		bbalance += bsum; bstreaks += bsum = (bsum == 0) | (bsum == 32);
		cbalance += csum; cstreaks += csum = (csum == 0) | (csum == 32);
		dbalance += dsum; dstreaks += dsum = (dsum == 0) | (dsum == 32);

		if (cnt > 516 && asum + bsum + csum + dsum == 0)
		{
			abalance += 48; pta += 96;
			bbalance += 48; ptb += 96;
			cbalance += 48; ptc += 96;
			dbalance += 48; ptd += 96;
			cnt -= 384;
		}
	}

	for ( ; cnt > 7 ; cnt -= 4)
	{
		abalance += cmp(pta, pta + 1) > 0; pta++;
		bbalance += cmp(ptb, ptb + 1) > 0; ptb++;
		cbalance += cmp(ptc, ptc + 1) > 0; ptc++;
		dbalance += cmp(ptd, ptd + 1) > 0; ptd++;
	}

	if (quad1 < quad2) {bbalance += cmp(ptb, ptb + 1) > 0; ptb++;}
	if (quad1 < quad3) {cbalance += cmp(ptc, ptc + 1) > 0; ptc++;}
	if (quad1 < quad4) {dbalance += cmp(ptd, ptd + 1) > 0; ptd++;}

	cnt = abalance + bbalance + cbalance + dbalance;

	if (cnt == 0)
	{
		if (cmp(pta, pta + 1) <= 0 && cmp(ptb, ptb + 1) <= 0 && cmp(ptc, ptc + 1) <= 0)
		{
			return;
		}
	}

	asum = quad1 - abalance == 1;
	bsum = quad2 - bbalance == 1;
	csum = quad3 - cbalance == 1;
	dsum = quad4 - dbalance == 1;

	if (asum | bsum | csum | dsum)
	{
		unsigned char span1 = (asum && bsum) * (cmp(pta, pta + 1) > 0);
		unsigned char span2 = (bsum && csum) * (cmp(ptb, ptb + 1) > 0);
		unsigned char span3 = (csum && dsum) * (cmp(ptc, ptc + 1) > 0);

		switch (span1 | span2 * 2 | span3 * 4)
		{
			case 0: break;
			case 1: FUNC(quad_reversal)(array, ptb);   abalance = bbalance = 0; break;
			case 2: FUNC(quad_reversal)(pta + 1, ptc); bbalance = cbalance = 0; break;
			case 3: FUNC(quad_reversal)(array, ptc);   abalance = bbalance = cbalance = 0; break;
			case 4: FUNC(quad_reversal)(ptb + 1, ptd); cbalance = dbalance = 0; break;
			case 5: FUNC(quad_reversal)(array, ptb);
				FUNC(quad_reversal)(ptb + 1, ptd); abalance = bbalance = cbalance = dbalance = 0; break;
			case 6: FUNC(quad_reversal)(pta + 1, ptd); bbalance = cbalance = dbalance = 0; break;
			case 7: FUNC(quad_reversal)(array, ptd); return;
		}

		if (asum && abalance) {FUNC(quad_reversal)(array,   pta); abalance = 0;}
		if (bsum && bbalance) {FUNC(quad_reversal)(pta + 1, ptb); bbalance = 0;}
		if (csum && cbalance) {FUNC(quad_reversal)(ptb + 1, ptc); cbalance = 0;}
		if (dsum && dbalance) {FUNC(quad_reversal)(ptc + 1, ptd); dbalance = 0;}
	}

#ifdef cmp
	cnt = nmemb / 256; // more than 50% ordered
#else
	cnt = nmemb / 512; // more than 25% ordered
#endif
	asum = astreaks > cnt;
	bsum = bstreaks > cnt;
	csum = cstreaks > cnt;
	dsum = dstreaks > cnt;

#ifndef cmp
	if (quad1 > QUAD_CACHE)
	{
		asum = bsum = csum = dsum = 1;
	}
#endif
	switch (asum + bsum * 2 + csum * 4 + dsum * 8)
	{
		case 0:
			FUNC(blit_partition)(array, swap, swap_size, nmemb, cmp);
			return;
		case 1:
			if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			FUNC(blit_partition)(pta + 1, swap, swap_size, quad2 + half2, cmp);
			break;
		case 2:
			FUNC(blit_partition)(array, swap, swap_size, quad1, cmp);
			if (bbalance) FUNC(quadsort_swap)(pta + 1, swap, swap_size, quad2, cmp);
			FUNC(blit_partition)(ptb + 1, swap, swap_size, half2, cmp);
			break;
		case 3:
			if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			if (bbalance) FUNC(quadsort_swap)(pta + 1, swap, swap_size, quad2, cmp);
			FUNC(blit_partition)(ptb + 1, swap, swap_size, half2, cmp);
			break;
		case 4:
			FUNC(blit_partition)(array, swap, swap_size, half1, cmp);
			if (cbalance) FUNC(quadsort_swap)(ptb + 1, swap, swap_size, quad3, cmp);
			FUNC(blit_partition)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 8:
			FUNC(blit_partition)(array, swap, swap_size, half1 + quad3, cmp);
			if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 9:
			if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			FUNC(blit_partition)(pta + 1, swap, swap_size, quad2 + quad3, cmp);
			if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 12:
			FUNC(blit_partition)(array, swap, swap_size, half1, cmp);
			if (cbalance) FUNC(quadsort_swap)(ptb + 1, swap, swap_size, quad3, cmp);
			if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 5:
		case 6:
		case 7:
		case 10:
		case 11:
		case 13:
		case 14:
		case 15:
			if (asum)
			{
				if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			}
			else FUNC(blit_partition)(array, swap, swap_size, quad1, cmp);
			if (bsum)
			{
				if (bbalance) FUNC(quadsort_swap)(pta + 1, swap, swap_size, quad2, cmp);
			}
			else FUNC(blit_partition)(pta + 1, swap, swap_size, quad2, cmp);
			if (csum)
			{
				if (cbalance) FUNC(quadsort_swap)(ptb + 1, swap, swap_size, quad3, cmp);
			}
			else FUNC(blit_partition)(ptb + 1, swap, swap_size, quad3, cmp);
			if (dsum)
			{
				if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			}
			else FUNC(blit_partition)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
	}

	if (cmp(pta, pta + 1) <= 0)
	{
		if (cmp(ptc, ptc + 1) <= 0)
		{
			if (cmp(ptb, ptb + 1) <= 0)
			{
				return;
			}
		}
		else
		{
			FUNC(blit_merge_block)(array + half1, swap, swap_size, quad3, quad4, cmp);
		}
	}
	else
	{
		FUNC(blit_merge_block)(array, swap, swap_size, quad1, quad2, cmp);

		if (cmp(ptc, ptc + 1) > 0)
		{
			FUNC(blit_merge_block)(array + half1, swap, swap_size, quad3, quad4, cmp);
		}
	}
	FUNC(blit_merge_block)(array, swap, swap_size, half1, half2, cmp);
}

VAR FUNC(blit_median_of_sqrt)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	VAR *pta, *pts;
	size_t cnt, sqrt, div;

	sqrt = nmemb > 262144 ? 256 : 128;

	div = nmemb / sqrt;

	pta = array + rand() % sqrt;
	pts = swap;

	for (cnt = 0 ; cnt < sqrt ; cnt++)
	{
		pts[cnt] = pta[0];

		pta += div;
	}
	FUNC(quadsort_swap)(pts, pts + sqrt, sqrt, sqrt, cmp);

	return pts[sqrt / 2];
}

VAR FUNC(blit_median_of_five)(VAR *array, size_t v0, size_t v1, size_t v2, size_t v3, size_t v4, CMPFUNC *cmp)
{
	VAR swap[6], *pta;
	size_t x, y, z;

	swap[2] = array[v0];
	swap[3] = array[v1];
	swap[4] = array[v2];
	swap[5] = array[v3];

	pta = swap + 2;

	x = cmp(pta, pta + 1) > 0; y = !x; swap[0] = pta[y]; pta[0] = pta[x]; pta[1] = swap[0]; pta += 2;
	x = cmp(pta, pta + 1) > 0; y = !x; swap[0] = pta[y]; pta[0] = pta[x]; pta[1] = swap[0]; pta -= 2;
	x = cmp(pta, pta + 2) > 0; y = !x; swap[0] = pta[0]; swap[1] = pta[2]; pta[0] = swap[x]; pta[2] = swap[y]; pta++;
	x = cmp(pta, pta + 2) > 0; y = !x; swap[0] = pta[0]; swap[1] = pta[2]; pta[0] = swap[x]; pta[2] = swap[y];

	pta[2] = array[v4];

	x = cmp(pta, pta + 1) > 0;
	y = cmp(pta, pta + 2) > 0;
	z = cmp(pta + 1, pta + 2) > 0;

	return pta[(x == y) + (y ^ z)];
}

VAR FUNC(blit_median_of_twentyfive)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap[5];
	size_t div = nmemb / 64;

	swap[0] = FUNC(blit_median_of_five)(array, div *  4, div *  1, div *  2, div *  8, div * 10, cmp);
	swap[1] = FUNC(blit_median_of_five)(array, div * 16, div * 12, div * 14, div * 18, div * 20, cmp);
	swap[2] = FUNC(blit_median_of_five)(array, div * 32, div * 24, div * 30, div * 34, div * 38, cmp);
	swap[3] = FUNC(blit_median_of_five)(array, div * 48, div * 42, div * 44, div * 50, div * 52, cmp);
	swap[4] = FUNC(blit_median_of_five)(array, div * 60, div * 54, div * 56, div * 62, div * 63, cmp);

	return FUNC(blit_median_of_five)(swap, 0, 1, 2, 3, 4, cmp);
}

size_t FUNC(blit_median_of_three)(VAR *array, size_t v0, size_t v1, size_t v2, CMPFUNC *cmp)
{
	size_t v[3] = {v0, v1, v2};
	char x, y, z;

	x = cmp(array + v0, array + v1) > 0;
	y = cmp(array + v0, array + v2) > 0;
	z = cmp(array + v1, array + v2) > 0;

	return v[(x == y) + (y ^ z)];
}

VAR FUNC(blit_median_of_nine)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	size_t x, y, z, div = nmemb / 16;

	x = FUNC(blit_median_of_three)(array, div * 2, div * 1, div * 4, cmp);
	y = FUNC(blit_median_of_three)(array, div * 8, div * 6, div * 10, cmp);
	z = FUNC(blit_median_of_three)(array, div * 14, div * 12, div * 15, cmp);

	return array[FUNC(blit_median_of_three)(array, x, y, z, cmp)];
}

// As per suggestion by Marshall Lochbaum to improve generic data handling

size_t FUNC(blit_reverse_partition)(VAR *array, VAR *swap, VAR *piv, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb > swap_size)
	{
		size_t l, r, h = nmemb / 2;

		l = FUNC(blit_reverse_partition)(array + 0, swap, piv, swap_size, h, cmp);
		r = FUNC(blit_reverse_partition)(array + h, swap, piv, swap_size, nmemb - h, cmp);

		FUNC(trinity_rotation)(array + l, swap, swap_size, h - l + r, h - l);

		return l + r;
	}
	size_t cnt, val, m;
	VAR *pta = array;

	for (m = 0, cnt = nmemb / 4 ; cnt ; cnt--)
	{
		val = cmp(piv, pta) > 0; swap[-m] = array[m] = *pta++; m += val; swap++;
		val = cmp(piv, pta) > 0; swap[-m] = array[m] = *pta++; m += val; swap++;
		val = cmp(piv, pta) > 0; swap[-m] = array[m] = *pta++; m += val; swap++;
		val = cmp(piv, pta) > 0; swap[-m] = array[m] = *pta++; m += val; swap++;
	}

	for (cnt = nmemb % 4 ; cnt ; cnt--)
	{
		val = cmp(piv, pta) > 0; swap[-m] = array[m] = *pta++; m += val; swap++;
	}

	memcpy(array + m, swap - nmemb, (nmemb - m) * sizeof(VAR));

	return m;
}

size_t FUNC(blit_default_partition)(VAR *array, VAR *swap, VAR *piv, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb > swap_size)
	{
		size_t l, r, h = nmemb / 2;

		l = FUNC(blit_default_partition)(array + 0, swap, piv, swap_size, h, cmp);
		r = FUNC(blit_default_partition)(array + h, swap, piv, swap_size, nmemb - h, cmp);

		FUNC(trinity_rotation)(array + l, swap, swap_size, h - l + r, h - l);

		return l + r;
	}
	size_t cnt, val, m = 0;
	VAR *pta = array;

	for (cnt = nmemb / 4 ; cnt ; cnt--)
	{
		val = cmp(pta, piv) <= 0; swap[-m] = array[m] = *pta++; m += val; swap++;
		val = cmp(pta, piv) <= 0; swap[-m] = array[m] = *pta++; m += val; swap++;
		val = cmp(pta, piv) <= 0; swap[-m] = array[m] = *pta++; m += val; swap++;
		val = cmp(pta, piv) <= 0; swap[-m] = array[m] = *pta++; m += val; swap++;
	}

	for (cnt = nmemb % 4 ; cnt ; cnt--)
	{
		val = cmp(pta, piv) <= 0; swap[-m] = array[m] = *pta++; m += val; swap++;
	}

	memcpy(array + m, swap - nmemb, sizeof(VAR) * (nmemb - m));

	return m;
}

void FUNC(blit_partition)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	size_t a_size = 0, s_size;
	VAR piv, max;

	while (1)
	{
		if (nmemb <= 2048)
		{
			piv = FUNC(blit_median_of_nine)(array, nmemb, cmp);
		}
		else if (nmemb <= 65536 || swap_size < 512)
		{
			piv = FUNC(blit_median_of_twentyfive)(array, nmemb, cmp);
		}
		else
		{
			piv = FUNC(blit_median_of_sqrt)(array, swap, swap_size, nmemb, cmp);
		}

		if (a_size && cmp(&max, &piv) <= 0)
		{
			a_size = FUNC(blit_reverse_partition)(array, swap, &piv, swap_size, nmemb, cmp);
			s_size = nmemb - a_size;

			if (s_size <= a_size / 16 || a_size <= BLIT_OUT)
			{
				return FUNC(quadsort_swap)(array, swap, swap_size, a_size, cmp);
			}
			nmemb = a_size; a_size = 0;
			continue;
		}

		a_size = FUNC(blit_default_partition)(array, swap, &piv, swap_size, nmemb, cmp);
		s_size = nmemb - a_size;

		if (a_size <= s_size / 16 || s_size <= BLIT_OUT)
		{
			if (s_size == 0)
			{
				a_size = FUNC(blit_reverse_partition)(array, swap, &piv, swap_size, a_size, cmp);
				s_size = nmemb - a_size;

				if (s_size <= a_size / 16 || a_size <= BLIT_OUT)
				{
					return FUNC(quadsort_swap)(array, swap, swap_size, a_size, cmp);
				}
				nmemb = a_size; a_size = 0;
				continue;
			}
			FUNC(quadsort_swap)(array + a_size, swap, swap_size, s_size, cmp);
		}
		else
		{
			FUNC(blit_partition)(array + a_size, swap, swap_size, s_size, cmp);
		}

		if (s_size <= a_size / 16 || a_size <= BLIT_OUT)
		{
			return FUNC(quadsort_swap)(array, swap, swap_size, a_size, cmp);
		}
		nmemb = a_size;
		max = piv;
	}
}

void FUNC(blitsort)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb <= 132)
	{
		return FUNC(quadsort)(array, nmemb, cmp);
	}
	else
	{
#if BLIT_AUX
		size_t swap_size = BLIT_AUX;
#else
		size_t swap_size = 1 << 19;

		while (nmemb / swap_size < swap_size / 32)
		{
			swap_size /= 4;
		}
		printf("swap_size: %lu\n", swap_size);
#endif
		VAR swap[swap_size];

		FUNC(blit_analyze)(array, swap, swap_size, nmemb, cmp);
	}
}

void FUNC(blitsort_swap)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb <= 132)
	{
		FUNC(quadsort_swap)(array, swap, swap_size, nmemb, cmp);
	}
	else
	{
		FUNC(blit_analyze)(array, swap, swap_size, nmemb, cmp);
	}
}

#undef BLIT_AUX
#undef BLIT_OUT
