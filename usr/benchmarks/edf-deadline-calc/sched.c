/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdlib.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/time.h>
#include <etaos/tick.h>

#include <asm/pgm.h>

#define edf_prio_ratio(__p) (((13 * __p) / 24) + 10)

static const unsigned char __prio_array_inmem[] = {
	edf_prio_ratio(0), edf_prio_ratio(1), edf_prio_ratio(2),
	edf_prio_ratio(3), edf_prio_ratio(4), edf_prio_ratio(5),
	edf_prio_ratio(6), edf_prio_ratio(7), edf_prio_ratio(8),
	edf_prio_ratio(9), edf_prio_ratio(10), edf_prio_ratio(11),
	edf_prio_ratio(12), edf_prio_ratio(13), edf_prio_ratio(14),
	edf_prio_ratio(15), edf_prio_ratio(16), edf_prio_ratio(17),
	edf_prio_ratio(18), edf_prio_ratio(19), edf_prio_ratio(20),
	edf_prio_ratio(21), edf_prio_ratio(22), edf_prio_ratio(23),
	edf_prio_ratio(24), edf_prio_ratio(25), edf_prio_ratio(26),
	edf_prio_ratio(27), edf_prio_ratio(28), edf_prio_ratio(29),
	edf_prio_ratio(30), edf_prio_ratio(31), edf_prio_ratio(32),
	edf_prio_ratio(33), edf_prio_ratio(34), edf_prio_ratio(35),
	edf_prio_ratio(36), edf_prio_ratio(37), edf_prio_ratio(38),
	edf_prio_ratio(39), edf_prio_ratio(40), edf_prio_ratio(41),
	edf_prio_ratio(42), edf_prio_ratio(43), edf_prio_ratio(44),
	edf_prio_ratio(45), edf_prio_ratio(46), edf_prio_ratio(47),
	edf_prio_ratio(48), edf_prio_ratio(49), edf_prio_ratio(50),
	edf_prio_ratio(51), edf_prio_ratio(52), edf_prio_ratio(53),
	edf_prio_ratio(54), edf_prio_ratio(55), edf_prio_ratio(56),
	edf_prio_ratio(57), edf_prio_ratio(58), edf_prio_ratio(59),
	edf_prio_ratio(60), edf_prio_ratio(61), edf_prio_ratio(62),
	edf_prio_ratio(63), edf_prio_ratio(64), edf_prio_ratio(65),
	edf_prio_ratio(66), edf_prio_ratio(67), edf_prio_ratio(68),
	edf_prio_ratio(69), edf_prio_ratio(70), edf_prio_ratio(71),
	edf_prio_ratio(72), edf_prio_ratio(73), edf_prio_ratio(74),
	edf_prio_ratio(75), edf_prio_ratio(76), edf_prio_ratio(77),
	edf_prio_ratio(78), edf_prio_ratio(79), edf_prio_ratio(80),
	edf_prio_ratio(81), edf_prio_ratio(82), edf_prio_ratio(83),
	edf_prio_ratio(84), edf_prio_ratio(85), edf_prio_ratio(86),
	edf_prio_ratio(87), edf_prio_ratio(88), edf_prio_ratio(89),
	edf_prio_ratio(90), edf_prio_ratio(91), edf_prio_ratio(92),
	edf_prio_ratio(93), edf_prio_ratio(94), edf_prio_ratio(95),
	edf_prio_ratio(96), edf_prio_ratio(97), edf_prio_ratio(98),
	edf_prio_ratio(99), edf_prio_ratio(100), edf_prio_ratio(101),
	edf_prio_ratio(102), edf_prio_ratio(103), edf_prio_ratio(104),
	edf_prio_ratio(105), edf_prio_ratio(106), edf_prio_ratio(107),
	edf_prio_ratio(108), edf_prio_ratio(109), edf_prio_ratio(110),
	edf_prio_ratio(111), edf_prio_ratio(112), edf_prio_ratio(113),
	edf_prio_ratio(114), edf_prio_ratio(115), edf_prio_ratio(116),
	edf_prio_ratio(117), edf_prio_ratio(118), edf_prio_ratio(119),
	edf_prio_ratio(120), edf_prio_ratio(121), edf_prio_ratio(122),
	edf_prio_ratio(123), edf_prio_ratio(124), edf_prio_ratio(125),
	edf_prio_ratio(126), edf_prio_ratio(127), edf_prio_ratio(128),
	edf_prio_ratio(129), edf_prio_ratio(130), edf_prio_ratio(131),
	edf_prio_ratio(132), edf_prio_ratio(133), edf_prio_ratio(134),
	edf_prio_ratio(135), edf_prio_ratio(136), edf_prio_ratio(137),
	edf_prio_ratio(138), edf_prio_ratio(139), edf_prio_ratio(140),
	edf_prio_ratio(141), edf_prio_ratio(142), edf_prio_ratio(143),
	edf_prio_ratio(144), edf_prio_ratio(145), edf_prio_ratio(146),
	edf_prio_ratio(147), edf_prio_ratio(148), edf_prio_ratio(149),
	edf_prio_ratio(150), edf_prio_ratio(151), edf_prio_ratio(152),
	edf_prio_ratio(153), edf_prio_ratio(154), edf_prio_ratio(155),
	edf_prio_ratio(156), edf_prio_ratio(157), edf_prio_ratio(158),
	edf_prio_ratio(159), edf_prio_ratio(160), edf_prio_ratio(161),
	edf_prio_ratio(162), edf_prio_ratio(163), edf_prio_ratio(164),
	edf_prio_ratio(165), edf_prio_ratio(166), edf_prio_ratio(167),
	edf_prio_ratio(168), edf_prio_ratio(169), edf_prio_ratio(170),
	edf_prio_ratio(171), edf_prio_ratio(172), edf_prio_ratio(173),
	edf_prio_ratio(174), edf_prio_ratio(175), edf_prio_ratio(176),
	edf_prio_ratio(177), edf_prio_ratio(178), edf_prio_ratio(179),
	edf_prio_ratio(180), edf_prio_ratio(181), edf_prio_ratio(182),
	edf_prio_ratio(183), edf_prio_ratio(184), edf_prio_ratio(185),
	edf_prio_ratio(186), edf_prio_ratio(187), edf_prio_ratio(188),
	edf_prio_ratio(189), edf_prio_ratio(190), edf_prio_ratio(191),
	edf_prio_ratio(192), edf_prio_ratio(193), edf_prio_ratio(194),
	edf_prio_ratio(195), edf_prio_ratio(196), edf_prio_ratio(197),
	edf_prio_ratio(198), edf_prio_ratio(199), edf_prio_ratio(200),
	edf_prio_ratio(201), edf_prio_ratio(202), edf_prio_ratio(203),
	edf_prio_ratio(204), edf_prio_ratio(205), edf_prio_ratio(206),
	edf_prio_ratio(207), edf_prio_ratio(208), edf_prio_ratio(209),
	edf_prio_ratio(210), edf_prio_ratio(211), edf_prio_ratio(212),
	edf_prio_ratio(213), edf_prio_ratio(214), edf_prio_ratio(215),
	edf_prio_ratio(216), edf_prio_ratio(217), edf_prio_ratio(218),
	edf_prio_ratio(219), edf_prio_ratio(220), edf_prio_ratio(221),
	edf_prio_ratio(222), edf_prio_ratio(223), edf_prio_ratio(224),
	edf_prio_ratio(225), edf_prio_ratio(226), edf_prio_ratio(227),
	edf_prio_ratio(228), edf_prio_ratio(229), edf_prio_ratio(230),
	edf_prio_ratio(231), edf_prio_ratio(232), edf_prio_ratio(233),
	edf_prio_ratio(234), edf_prio_ratio(235), edf_prio_ratio(236),
	edf_prio_ratio(237), edf_prio_ratio(238), edf_prio_ratio(239),
	edf_prio_ratio(240), edf_prio_ratio(241), edf_prio_ratio(242),
	edf_prio_ratio(243), edf_prio_ratio(244), edf_prio_ratio(245),
	edf_prio_ratio(246), edf_prio_ratio(247), edf_prio_ratio(248),
	edf_prio_ratio(249), edf_prio_ratio(250), edf_prio_ratio(251),
	edf_prio_ratio(252), edf_prio_ratio(253), edf_prio_ratio(254),
	edf_prio_ratio(255)
};

static const unsigned char __pgm __prio_array[] = {
	edf_prio_ratio(0), edf_prio_ratio(1), edf_prio_ratio(2),
	edf_prio_ratio(3), edf_prio_ratio(4), edf_prio_ratio(5),
	edf_prio_ratio(6), edf_prio_ratio(7), edf_prio_ratio(8),
	edf_prio_ratio(9), edf_prio_ratio(10), edf_prio_ratio(11),
	edf_prio_ratio(12), edf_prio_ratio(13), edf_prio_ratio(14),
	edf_prio_ratio(15), edf_prio_ratio(16), edf_prio_ratio(17),
	edf_prio_ratio(18), edf_prio_ratio(19), edf_prio_ratio(20),
	edf_prio_ratio(21), edf_prio_ratio(22), edf_prio_ratio(23),
	edf_prio_ratio(24), edf_prio_ratio(25), edf_prio_ratio(26),
	edf_prio_ratio(27), edf_prio_ratio(28), edf_prio_ratio(29),
	edf_prio_ratio(30), edf_prio_ratio(31), edf_prio_ratio(32),
	edf_prio_ratio(33), edf_prio_ratio(34), edf_prio_ratio(35),
	edf_prio_ratio(36), edf_prio_ratio(37), edf_prio_ratio(38),
	edf_prio_ratio(39), edf_prio_ratio(40), edf_prio_ratio(41),
	edf_prio_ratio(42), edf_prio_ratio(43), edf_prio_ratio(44),
	edf_prio_ratio(45), edf_prio_ratio(46), edf_prio_ratio(47),
	edf_prio_ratio(48), edf_prio_ratio(49), edf_prio_ratio(50),
	edf_prio_ratio(51), edf_prio_ratio(52), edf_prio_ratio(53),
	edf_prio_ratio(54), edf_prio_ratio(55), edf_prio_ratio(56),
	edf_prio_ratio(57), edf_prio_ratio(58), edf_prio_ratio(59),
	edf_prio_ratio(60), edf_prio_ratio(61), edf_prio_ratio(62),
	edf_prio_ratio(63), edf_prio_ratio(64), edf_prio_ratio(65),
	edf_prio_ratio(66), edf_prio_ratio(67), edf_prio_ratio(68),
	edf_prio_ratio(69), edf_prio_ratio(70), edf_prio_ratio(71),
	edf_prio_ratio(72), edf_prio_ratio(73), edf_prio_ratio(74),
	edf_prio_ratio(75), edf_prio_ratio(76), edf_prio_ratio(77),
	edf_prio_ratio(78), edf_prio_ratio(79), edf_prio_ratio(80),
	edf_prio_ratio(81), edf_prio_ratio(82), edf_prio_ratio(83),
	edf_prio_ratio(84), edf_prio_ratio(85), edf_prio_ratio(86),
	edf_prio_ratio(87), edf_prio_ratio(88), edf_prio_ratio(89),
	edf_prio_ratio(90), edf_prio_ratio(91), edf_prio_ratio(92),
	edf_prio_ratio(93), edf_prio_ratio(94), edf_prio_ratio(95),
	edf_prio_ratio(96), edf_prio_ratio(97), edf_prio_ratio(98),
	edf_prio_ratio(99), edf_prio_ratio(100), edf_prio_ratio(101),
	edf_prio_ratio(102), edf_prio_ratio(103), edf_prio_ratio(104),
	edf_prio_ratio(105), edf_prio_ratio(106), edf_prio_ratio(107),
	edf_prio_ratio(108), edf_prio_ratio(109), edf_prio_ratio(110),
	edf_prio_ratio(111), edf_prio_ratio(112), edf_prio_ratio(113),
	edf_prio_ratio(114), edf_prio_ratio(115), edf_prio_ratio(116),
	edf_prio_ratio(117), edf_prio_ratio(118), edf_prio_ratio(119),
	edf_prio_ratio(120), edf_prio_ratio(121), edf_prio_ratio(122),
	edf_prio_ratio(123), edf_prio_ratio(124), edf_prio_ratio(125),
	edf_prio_ratio(126), edf_prio_ratio(127), edf_prio_ratio(128),
	edf_prio_ratio(129), edf_prio_ratio(130), edf_prio_ratio(131),
	edf_prio_ratio(132), edf_prio_ratio(133), edf_prio_ratio(134),
	edf_prio_ratio(135), edf_prio_ratio(136), edf_prio_ratio(137),
	edf_prio_ratio(138), edf_prio_ratio(139), edf_prio_ratio(140),
	edf_prio_ratio(141), edf_prio_ratio(142), edf_prio_ratio(143),
	edf_prio_ratio(144), edf_prio_ratio(145), edf_prio_ratio(146),
	edf_prio_ratio(147), edf_prio_ratio(148), edf_prio_ratio(149),
	edf_prio_ratio(150), edf_prio_ratio(151), edf_prio_ratio(152),
	edf_prio_ratio(153), edf_prio_ratio(154), edf_prio_ratio(155),
	edf_prio_ratio(156), edf_prio_ratio(157), edf_prio_ratio(158),
	edf_prio_ratio(159), edf_prio_ratio(160), edf_prio_ratio(161),
	edf_prio_ratio(162), edf_prio_ratio(163), edf_prio_ratio(164),
	edf_prio_ratio(165), edf_prio_ratio(166), edf_prio_ratio(167),
	edf_prio_ratio(168), edf_prio_ratio(169), edf_prio_ratio(170),
	edf_prio_ratio(171), edf_prio_ratio(172), edf_prio_ratio(173),
	edf_prio_ratio(174), edf_prio_ratio(175), edf_prio_ratio(176),
	edf_prio_ratio(177), edf_prio_ratio(178), edf_prio_ratio(179),
	edf_prio_ratio(180), edf_prio_ratio(181), edf_prio_ratio(182),
	edf_prio_ratio(183), edf_prio_ratio(184), edf_prio_ratio(185),
	edf_prio_ratio(186), edf_prio_ratio(187), edf_prio_ratio(188),
	edf_prio_ratio(189), edf_prio_ratio(190), edf_prio_ratio(191),
	edf_prio_ratio(192), edf_prio_ratio(193), edf_prio_ratio(194),
	edf_prio_ratio(195), edf_prio_ratio(196), edf_prio_ratio(197),
	edf_prio_ratio(198), edf_prio_ratio(199), edf_prio_ratio(200),
	edf_prio_ratio(201), edf_prio_ratio(202), edf_prio_ratio(203),
	edf_prio_ratio(204), edf_prio_ratio(205), edf_prio_ratio(206),
	edf_prio_ratio(207), edf_prio_ratio(208), edf_prio_ratio(209),
	edf_prio_ratio(210), edf_prio_ratio(211), edf_prio_ratio(212),
	edf_prio_ratio(213), edf_prio_ratio(214), edf_prio_ratio(215),
	edf_prio_ratio(216), edf_prio_ratio(217), edf_prio_ratio(218),
	edf_prio_ratio(219), edf_prio_ratio(220), edf_prio_ratio(221),
	edf_prio_ratio(222), edf_prio_ratio(223), edf_prio_ratio(224),
	edf_prio_ratio(225), edf_prio_ratio(226), edf_prio_ratio(227),
	edf_prio_ratio(228), edf_prio_ratio(229), edf_prio_ratio(230),
	edf_prio_ratio(231), edf_prio_ratio(232), edf_prio_ratio(233),
	edf_prio_ratio(234), edf_prio_ratio(235), edf_prio_ratio(236),
	edf_prio_ratio(237), edf_prio_ratio(238), edf_prio_ratio(239),
	edf_prio_ratio(240), edf_prio_ratio(241), edf_prio_ratio(242),
	edf_prio_ratio(243), edf_prio_ratio(244), edf_prio_ratio(245),
	edf_prio_ratio(246), edf_prio_ratio(247), edf_prio_ratio(248),
	edf_prio_ratio(249), edf_prio_ratio(250), edf_prio_ratio(251),
	edf_prio_ratio(252), edf_prio_ratio(253), edf_prio_ratio(254),
	edf_prio_ratio(255)
};

static inline int edf_lookup_ratio(unsigned char prio)
{
	return pgm_read_byte(__prio_array + prio);
}

static inline int edf_calc_ratio(unsigned char prio)
{
	return ((13 * prio) / 24) + 10;
}

volatile unsigned char storage[2];

void __used test_lookup_speed(void)
{
	int idx = 0;
	volatile unsigned char prio = 0;

	for(; idx < 20000; idx++) {
		prio = random_m(255);
		storage[1] = edf_lookup_ratio(prio);
	}
}

void __used test_basic_lookup_speed(void)
{
	int idx = 0;
	volatile unsigned char prio = 0;

	for(; idx < 20000; idx++) {
		prio = random_m(255);
		storage[1] = __prio_array_inmem[prio];
	}
}

void __used test_calc_speed(void)
{
	int idx = 0;
	volatile unsigned char prio = 0;

	for(; idx < 20000; idx++) {
		prio = random_m(255);
		storage[0] = edf_calc_ratio(prio);
	}
}

int main(void)
{
	int64_t tick_orig, tick_end, tick_between1, tick_between2;
	int test1_duration, test2_duration, test3_duration;

	printf_P(PSTR("Application started!\n"));

	tick_orig = sys_tick;
	test_calc_speed();
	tick_between1 = sys_tick;
	test_lookup_speed();
	tick_between2 = sys_tick;
	test_basic_lookup_speed();
	tick_end = sys_tick;

	test1_duration = tick_between1 - tick_orig;
	test2_duration = tick_between2 - tick_between1;
	test3_duration = tick_end - tick_between2;

	printf_P(PSTR("\nResults (20 000 lookups):\n\n"));
	printf("Calculation test duration: %ims\n"
	       "Program memory test duration: %ims\n"
	       "Data memory test duration: %ims\n",
	       test1_duration, test2_duration, test3_duration);
	return -EOK;
}

