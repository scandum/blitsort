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
	blitsort 1.1.5.2
*/

#define BLIT_AUX 512 // set to 0 for sqrt N cache size
#define BLIT_OUT  24

size_t FUNC(blit_analyze)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	char loop, dist;
	size_t cnt, balance = 0, streaks = 0;
	VAR *pta, *ptb, tmp;

	pta = array;

	for (cnt = nmemb ; cnt > 16 ; cnt -= 16)
	{
		for (dist = 0, loop = 16 ; loop ; loop--)
		{
			dist += cmp(pta, pta + 1) > 0; pta++;
		}
		streaks += (dist == 0) | (dist == 16);
		balance += dist;
	}

	while (--cnt)
	{
		balance += cmp(pta, pta + 1) > 0;
		pta++;
	}

	if (balance == 0)
	{
		return 1;
	}

	if (balance == nmemb - 1)
	{
		pta = array;
		ptb = array + nmemb;

		cnt = nmemb / 2;

		do
		{
			tmp = *pta; *pta++ = *--ptb; *ptb = tmp;
		}
		while (--cnt);

		return 1;
	}

	if (streaks >= nmemb / 36)
	{
		FUNC(quadsort_swap)(array, swap, swap_size, nmemb, cmp);

		return 1;
	}
	return 0;
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
	if (nmemb < 32)
	{
		return FUNC(tail_swap)(array, nmemb, cmp);
	}
	else
	{
#if BLIT_AUX
		size_t swap_size = BLIT_AUX;
#else
		size_t swap_size = 32;

		while (swap_size * swap_size <= nmemb)
		{
			swap_size *= 4;
		}
#endif
		VAR swap[swap_size];

		if (FUNC(blit_analyze)(array, swap, swap_size, nmemb, cmp) == 0)
		{
			FUNC(blit_partition)(array, swap, swap_size, nmemb, cmp);
		}
	}
}

void FUNC(blitsort_swap)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (FUNC(blit_analyze)(array, swap, swap_size, nmemb, cmp) == 0)
	{
		FUNC(blit_partition)(array, swap, swap_size, nmemb, cmp);
	}
}

#undef BLIT_AUX
#undef BLIT_OUT
