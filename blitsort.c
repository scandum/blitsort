/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
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
	blitsort 1.1.4.2
*/

void FUNC(tail_swap)(VAR *array, unsigned char nmemb, CMPFUNC *cmp)
{
	VAR *pta, *end, *ptt, tmp;
	unsigned char mid, top, offset;

	switch (nmemb)
	{
		case 0:
		case 1:
			return;

		case 2:
			swap_two(array, tmp);
			return;

		case 3:
			swap_three(array, tmp);
			return;

		case 4:
			swap_four(array, tmp);
			return;

		case 5:
			swap_four(array, tmp);
			swap_five(array, pta, ptt, end, tmp, cmp);
			return;

		case 6:
			swap_four(array, tmp);
			swap_six(array, pta, ptt, end, tmp, cmp);
			return;

		case 7:
			swap_four(array, tmp);
			swap_seven(array, pta, ptt, end, tmp, cmp);
			return;
		case 8:
			swap_four(array, tmp);
			swap_eight(array, pta, ptt, end, tmp, cmp);
			return;
	}
	swap_four(array, tmp);
	swap_eight(array, pta, ptt, end, tmp, cmp);

	end = array + 8;
	offset = 8;

	while (offset < nmemb)
	{
		top = offset++;
		pta = end++;
		ptt = pta--;

		if (cmp(pta, ptt) <= 0)
		{
			continue;
		}

		tmp = *ptt;

		while (top > 1)
		{
			mid = top / 2;

			if (cmp(pta - mid, &tmp) > 0)
			{
				pta -= mid;
			}
			top -= mid;
		}

		memmove(pta + 1, pta, (ptt - pta) * sizeof(VAR));

		*pta = tmp;
	}
}

void FUNC(tail_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp);
void FUNC(parity_merge_thirtytwo)(VAR *array, VAR *swap, CMPFUNC *cmp);

size_t FUNC(quad_swap)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap[32];
	size_t count, reverse;
	VAR *pta, *pts, *ptt, *pte, tmp;

	pta = array;

	count = nmemb / 4;

	count &= ~1;

	while (count--)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			if (cmp(&pta[2], &pta[3]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					pts = pta;
					pta += 4;
					goto swapper;
				}
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
			tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
		}
		else if (cmp(&pta[2], &pta[3]) > 0)
		{
			tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
		}

		if (cmp(&pta[1], &pta[2]) > 0)
		{
			if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
			}
			else if (cmp(&pta[0], &pta[3]) > 0)
			{
				tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp; tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else if (cmp(&pta[1], &pta[3]) <= 0)
			{
				tmp = pta[1]; pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else
			{
				tmp = pta[1]; pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
		}
		count--;

		pts = pta;

		swap_eight(pts, ptt, pte, pta, tmp, cmp);

		continue;

		swapper:

		if (count--)
		{
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[2], &pta[3]) > 0)
				{
					if (cmp(&pta[1], &pta[2]) > 0)
					{
						if (cmp(&pta[-1], &pta[0]) > 0)
						{
							pta += 4;

							goto swapper;
						}
					}
					tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			else if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}

			if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) <= 0)
				{
					if (cmp(&pta[1], &pta[3]) <= 0)
					{
						tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
					}
					else
					{
						tmp = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
					}
				}
				else if (cmp(&pta[0], &pta[3]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp; tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;
				}
				else if (cmp(&pta[1], &pta[3]) <= 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = pta[1]; pta[1] = tmp;
				}
			}
			ptt = pta - 1;

			reverse = (ptt - pts) / 2;

			do
			{
				tmp = *pts; *pts++ = *ptt; *ptt-- = tmp;
			}
			while (reverse--);

			pts = count & 1 ? pta : pta - 4;

			count &= ~1;

			swap_eight(pts, ptt, pte, pta, tmp, cmp);

			continue;
		}

		if (pts == array)
		{
			switch (nmemb & 7)
			{
				case 7:
					if (cmp(&pta[5], &pta[6]) <= 0)
					{
						break;
					}
				case 6:
					if (cmp(&pta[4], &pta[5]) <= 0)
					{
						break;
					}
				case 5:
					if (cmp(&pta[3], &pta[4]) <= 0)
					{
						break;
					}
				case 4:
					if (cmp(&pta[2], &pta[3]) <= 0)
					{
						break;
					}
				case 3:
					if (cmp(&pta[1], &pta[2]) <= 0)
					{
						break;
					}
				case 2:
					if (cmp(&pta[0], &pta[1]) <= 0)
					{
						break;
					}
				case 1:
					if (cmp(&pta[-1], &pta[0]) <= 0)
					{
						break;
					}
				case 0:
					ptt = pts + nmemb - 1;

					reverse = (ptt - pts) / 2;

					do
					{
						tmp = *pts; *pts++ = *ptt; *ptt-- = tmp;
					}
					while (reverse--);

					return 1;
			}
		}
		ptt = pta - 1;

		reverse = (ptt - pts) / 2;

		do
		{
			tmp = *pts; *pts++ = *ptt; *ptt-- = tmp;
		}
		while (reverse--);

		break;
	}

	FUNC(tail_swap)(pta, nmemb & 7, cmp);

	pta = array;

	count = nmemb / 32;

	while (count--)
	{
		FUNC(parity_merge_thirtytwo)(pta, swap, cmp);

		pta += 32;
	}

	if ((nmemb & 31) > 8)
	{
		FUNC(tail_merge)(pta, swap, 32, nmemb & 31, 8, cmp);
	}

	return 0;
}

void FUNC(parity_merge_eight)(VAR *dest, VAR *from, CMPFUNC *cmp)
{
	VAR *ptl, *ptr;

	ptl = from;
	ptr = from + 8;

	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;

	ptl = from + 7;
	ptr = from + 15;

	dest += 8;

	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest = cmp(ptl, ptr) > 0 ? *ptl : *ptr;
}

void FUNC(parity_merge_sixteen)(VAR *dest, VAR *from, CMPFUNC *cmp)
{
	VAR *ptl, *ptr;

	ptl = from;
	ptr = from + 16;

	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	*dest = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;

	ptl = from + 15;
	ptr = from + 31;

	dest += 16;

	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest-- = cmp(ptl, ptr) > 0 ? *ptl-- : *ptr--;
	*dest = cmp(ptl, ptr) > 0 ? *ptl : *ptr;
}

void FUNC(parity_merge_thirtytwo)(VAR *array, VAR *swap, CMPFUNC *cmp)
{
	if (cmp(array + 7, array + 8) <= 0 && cmp(array + 15, array + 16) <= 0 && cmp(array + 23, array + 24) <= 0)
	{
		return;
	}
	FUNC(parity_merge_eight)(swap, array, cmp);
	FUNC(parity_merge_eight)(swap + 16, array + 16, cmp);

	FUNC(parity_merge_sixteen)(array, swap, cmp);
}

void FUNC(forward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *r, *m, *e, *s; // right, middle, end, swap

	r = array + block;
	e = array + nmemb - 1;

	memcpy(swap, array, block * sizeof(VAR));

	s = swap;
	m = swap + block - 1;

	if (cmp(m, e) <= 0)
	{
		do
		{
			while (cmp(s, r) > 0)
			{
				*array++ = *r++;
			}
			*array++ = *s++;
		}
		while (s <= m);
	}
	else
	{
		do
		{
			if (cmp(s, r) > 0)
			{
				*array++ = *r++;
				continue;
			}
			*array++ = *s++;

			if (cmp(s, r) > 0)
			{
				*array++ = *r++;
				continue;
			}
			*array++ = *s++;

			if (cmp(s, r) > 0)
			{
				*array++ = *r++;
				continue;
			}
			*array++ = *s++;
		}
		while (r <= e);

		do *array++ = *s++; while (s <= m);
	}
}

void FUNC(backward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *r, *m, *e, *s; // right, middle, end, swap

	m = array + block;
	e = array + nmemb - 1;
	r = m--;

	if (cmp(m, r) <= 0)
	{
		return;
	}

	while (cmp(m, e) <= 0)
	{
		e--;
	}

	s = swap;

	do *s++ = *r++; while (r <= e);

	s--;

	*e-- = *m--;

	if (cmp(array, swap) <= 0)
	{
		do
		{
			while (cmp(m, s) > 0)
			{
				*e-- = *m--;
			}
			*e-- = *s--;
		}
		while (s >= swap);
	}
	else
	{
		do
		{
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
				continue;
			}
			*e-- = *s--;
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
				continue;
			}
			*e-- = *s--;
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
				continue;
			}
			*e-- = *s--;
		}
		while (m >= array);

		do *e-- = *s--; while (s >= swap);
	}
}

void FUNC(tail_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	register VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb && block <= swap_size)
	{
		pta = array;

		for (pta = array ; pta + block < pte ; pta += block * 2)
		{
			if (pta + block * 2 < pte)
			{
				FUNC(backward_merge)(pta, swap, block * 2, block, cmp);

				continue;
			}
			FUNC(backward_merge)(pta, swap, pte - pta, block, cmp);

			break;
		}
		block *= 2;
	}
}

void FUNC(trinity_rotation)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t left)
{
	size_t bridge, right = nmemb - left;

	if (left < right)
	{
		if (left <= swap_size)
		{
			memcpy(swap, array, left * sizeof(VAR));
			memmove(array, array + left, right * sizeof(VAR));
			memcpy(array + right, swap, left * sizeof(VAR));
		}
		else
		{
			VAR *pta, *ptb, *ptc, *ptd;

			pta = array;
			ptb = pta + left;

			bridge = right - left;

			if (bridge <= swap_size && bridge > 2)
			{
				ptc = pta + right;
				ptd = ptc + left;

				memcpy(swap, ptb, bridge * sizeof(VAR));

				while (left--)
				{
					*--ptc = *--ptd; *ptd = *--ptb;
				}
				memcpy(pta, swap, bridge * sizeof(VAR));
			}
			else
			{
				ptc = ptb;
				ptd = ptc + right;

				bridge = left / 2;

				while (bridge--)
				{
					*swap = *--ptb; *ptb = *pta; *pta++ = *ptc; *ptc++ = *--ptd; *ptd = *swap;
				}

				bridge = (ptd - ptc) / 2;

				while (bridge--)
				{
					*swap = *ptc; *ptc++ = *--ptd; *ptd = *pta; *pta++ = *swap;
				}

				bridge = (ptd - pta) / 2;

				while (bridge--)
				{
					*swap = *pta; *pta++ = *--ptd; *ptd = *swap;
				}
			}
		}
	}
	else if (right < left)
	{
		if (right <= swap_size)
		{
			memcpy(swap, array + left, right * sizeof(VAR));
			memmove(array + right, array, left * sizeof(VAR));
			memcpy(array, swap, right * sizeof(VAR));
		}
		else
		{
			VAR *pta, *ptb, *ptc, *ptd;

			pta = array;
			ptb = pta + left;

			bridge = left - right;

			if (bridge <= swap_size && bridge > 2)
			{
				ptc = pta + right;
				ptd = ptc + left;

				memcpy(swap, ptc, bridge * sizeof(VAR));

				while (right--)
				{
					*ptc++ = *pta; *pta++ = *ptb++;
				}
				memcpy(ptd - bridge, swap, bridge * sizeof(VAR));
			}
			else
			{
				ptc = ptb;
				ptd = ptc + right;

				bridge = right / 2;

				while (bridge--)
				{
					*swap = *--ptb; *ptb = *pta; *pta++ = *ptc; *ptc++ = *--ptd; *ptd = *swap;
				}

				bridge = (ptb - pta) / 2;

				while (bridge--)
				{
					*swap = *--ptb; *ptb = *pta; *pta++ = *--ptd; *ptd = *swap;
				}

				bridge = (ptd - pta) / 2;

				while (bridge--)
				{
					*swap = *pta; *pta++ = *--ptd; *ptd = *swap;
				}
			}
		}
	}
	else
	{
		VAR *pta, *ptb;

		pta = array;
		ptb = pta + left;

		while (left--)
		{
			*swap = *pta; *pta++ = *ptb; *ptb++ = *swap;
		}
	}
}

size_t FUNC(monobound_binary_first)(VAR *array, VAR *value, size_t top, CMPFUNC *cmp)
{
	VAR *end;
	size_t mid;

	end = array + top;

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(value, end - mid) <= 0)
		{
			end -= mid;
		}
		top -= mid;
	}

	if (cmp(value, end - 1) <= 0)
	{
		end--;
	}
	return (end - array);
}

void FUNC(blit_merge_block)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	size_t left, right;

	if (cmp(array + block - 1, array + block) <= 0)
	{
		return;
	}

	left = FUNC(monobound_binary_first)(array + block, array + block / 2, nmemb - block, cmp);

	right = nmemb - (block + left);

	block /= 2;

	if (left)
	{
		FUNC(trinity_rotation)(array + block, swap, swap_size, block + left, block);

		if (block <= swap_size && block <= left)
		{
			FUNC(forward_merge)(array, swap, block + left, block, cmp);
		}
		else if (left <= swap_size)
		{
			FUNC(backward_merge)(array, swap, block + left, block, cmp);
		}
		else
		{
			FUNC(blit_merge_block)(array, swap, swap_size, block + left, block, cmp);
		}
	}

	if (right)
	{
		if (block <= swap_size && block <= right)
		{
			FUNC(forward_merge)(array + block + left, swap, block + right, block, cmp);
		}
		else if (right <= swap_size)
		{
			FUNC(backward_merge)(array + block + left, swap, block + right, block, cmp);
		}
		else
		{
			FUNC(blit_merge_block)(array + block + left, swap, swap_size, block + right, block, cmp);
		}
	}
}

void FUNC(blit_merge)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *pta, *pte;

	pte = array + nmemb;

	while (block < nmemb)
	{
		pta = array;

		for (pta = array ; pta + block < pte ; pta += block * 2)
		{
			if (pta + block * 2 < pte)
			{
				FUNC(blit_merge_block)(pta, swap, swap_size, block * 2, block, cmp);

				continue;
			}
			FUNC(blit_merge_block)(pta, swap, swap_size, pte - pta, block, cmp);

			break;
		}
		block *= 2;
	}
}

void FUNC(blitsort)(void *array, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (FUNC(quad_swap)(array, nmemb, cmp) == 0)
	{
#if BLITCACHE
		size_t swap_size = BLITCACHE;
#else
		size_t swap_size = 32;

		while (swap_size * swap_size < nmemb)
		{
			swap_size *= 2;
		}
#endif
		VAR swap[swap_size];

		FUNC(tail_merge)(array, swap, swap_size, nmemb, 32, cmp);

		if (nmemb > swap_size * 2)
		{
			FUNC(blit_merge)(array, swap, swap_size, nmemb, swap_size * 2, cmp);
		}
	}
}
