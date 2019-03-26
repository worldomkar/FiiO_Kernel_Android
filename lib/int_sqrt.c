/*
 * Copyright (C) 2013 Davidlohr Bueso <davidlohr.bueso@hp.com>
 *
 *  Based on the shift-and-subtract algorithm for computing integer
 *  square root from Guy L. Steele.
 */

#include <linux/kernel.h>
#include <linux/module.h>

/**
 * int_sqrt - rough approximation to sqrt
 * @x: integer of which to calculate the sqrt
 *
 * A very rough approximation to the sqrt() function.
 */
unsigned long int_sqrt(unsigned long x)
{
//	unsigned long op, res, one;
	unsigned long b, m, y = 0;

//	op = x;
//	res = 0;
	if (x <= 1)
		return x;

//	one = 1UL << (BITS_PER_LONG - 2);
//	while (one > op)
//		one >>= 2;
	m = 1UL << (BITS_PER_LONG - 2);
	while (m != 0) {
		b = y + m;
		y >>= 1;


//	while (one != 0) {
//		if (op >= res + one) {
//			op = op - (res + one);
//			res = res +  2 * one;
		if (x >= b) {
			x -= b;
			y += m;

		}
//		res /= 2;
//		one /= 4;
		m >>= 2;
	}
//	return res;
	return y;
}
EXPORT_SYMBOL(int_sqrt);
