int test_and_clear_bit(unsigned nr, unsigned long *ptr)
{
	register unsigned char *p = ((unsigned char *)ptr) + (nr / 8);
	register unsigned char mask = 1UL << (nr % 8);
	register unsigned char rv;

	rv = *p & mask;
	mask ~= mask;
	*p &= mask;

	return rv != 0;
}

