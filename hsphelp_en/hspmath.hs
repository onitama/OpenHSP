%dll
hspmath
%ver
3.4
%date
2011/05/10
%group
Mathematical function
%port
Win
Mac
Cli
Let
%author
HSPWiki
%note
Include hspmath.as.



%type
User-defined macro
%index
M_LOG2E
Logarithm of Napier numbers with base 2

%inst
A constant that represents the logarithm of a 2-based Napier number.
The logarithm of the number of Napiers with a base of 2 refers to log2 (m_e).

%group
Mathematical constant

%sample
#include "hspmath.as"
	a = 10.0
mes "The logarithm of a with a base of 2 is" + log2 (a) + "."
mes "This is also calculated as logf (a) * m_log2e:" + (logf (a) * m_log2e) + "\\ n"

mes "By multiplying m_log2e by" e-based logarithm "in this way,"
mes "You can get" the base 2 logarithm ". "
	stop

%href
M_LOG10E
M_E
log2

%type
User-defined macro
%index
M_LOG10E
Logarithm of Napier numbers with base 10

%inst
A constant that represents the logarithm of the number of Napiers with a base of 10.
The logarithm of the base 10 Napier number is log10 (m_e).

%group
Mathematical constant

%sample
#include "hspmath.as"
	a = m_e
mes "The logarithm of a with a base of 10 is" + log10 (a) + "."
mes "This is also calculated as logf (a) * m_log10e:" + (logf (a) * m_log10e) + "\\ n"

mes "By multiplying m_log10e by" e-based logarithm "in this way,"
mes "You can get the" base 10 logarithm ". "
	stop

%href
M_LOG2E
log10
M_E


%type
User-defined macro
%index
M_LN2
Logarithm of 2 with the base of Napier

%inst
A constant that represents the logarithm of 2 with the base Napier number.
The base 2 logarithm of the Napier number refers to logf (2).

%group
Mathematical constant

%href
M_LN10



%type
User-defined macro
%index
M_LN10
Logarithm of 10 with the base of Napier

%inst
A constant representing the logarithm of 10 with the base Napier number.
The base 10 logarithm of the Napier number refers to logf (10).

%group
Mathematical constant

%href
M_LN2


%type
User-defined macro
%index
M_E
Napier number

%inst
A constant representing the Napier number (base of natural logarithm) e.

%href
M_PI
M_LOG2E
M_LOG10E

%group
Mathematical constant

%url
http://ja.wikipedia.org/wiki/%E3%83%8D%E3%82%A4%E3%83%94%E3%82%A2%E6%95%B0


%type
User-defined macro
%index
M_SQRTPI
Square root of pi

%inst
A constant that represents the square root of pi.

%href
sqrt
M_PI
M_SQRT2
M_SQRT3

%group
Mathematical constant


%type
User-defined macro
%index
M_SQRT2
Square root of 2

%inst
A constant that represents the square root of 2.

%href
sqrt
M_SQRTPI
M_SQRT3

%group
Mathematical constant



%type
User-defined macro
%index
M_SQRT3
Power root of 3

%inst
A constant that represents the root of a power of 3.

%href
sqrt
M_SQRTPI
M_SQRT2

%group
Mathematical constant


%type
User-defined macro
%index
DBL_DIG
Valid digits in decimal

%inst
A constant that represents the number of valid decimal digits.

%group
double limit



%type
User-defined macro
%index
DBL_EPSILON
Difference between 1.0 and the next largest value

%inst
A constant that represents the difference between 1.0 and the next largest value.

%group
double limit



%type
User-defined macro
%index
DBL_MANT_DIG
Bit number of mantissa

%inst
A constant that represents the number of bits in the mantissa used to represent a real number.

%group
double limit


%type
User-defined macro
%index
DBL_MAX
Maximum real number

%inst
A constant that represents the maximum value that can be represented by a real number.

%group
double limit

%href
INT_MAX



%type
User-defined macro
%index
DBL_MAX_10_EXP
Maximum value of the exponent part in decimal

%inst
A constant that represents the maximum value of the exponent.

%href
DBL_MIN_10_EXP
DBL_MAX_EXP

%group
double limit



%type
User-defined macro
%index
DBL_MAX_EXP
Maximum value of the exponent part in binary

%inst
A constant that represents the maximum value of the exponent.

%group
double limit

%href
DBL_MAX_10_EXP
INT_MAX


%type
User-defined macro
%index
DBL_MIN
Minimum value above 0

%inst
A constant that represents the smallest value greater than 0.

%group
double limit



%type
User-defined macro
%index
DBL_MIN_10_EXP
Minimum value of exponent part in decimal number

%inst
A constant that represents the minimum value of the exponent.

%href
DBL_MAX_10_EXP
DBL_MIN_EXP

%group
double limit



%type
User-defined macro
%index
DBL_MIN_EXP
Minimum value of the exponent part in binary

%inst
A constant that represents the minimum value of the exponent.

%href
DBL_MIN_10_EXP

%group
double limit



%type
User-defined macro
%index
INT_DIGIT
Valid digits in binary

%inst
A constant that represents the number of valid digits in binary.

%href
INT_DIGIT10

%group
int limit



%type
User-defined macro
%index
INT_DIGIT10
Valid digits in decimal

%inst
A constant that represents the number of valid decimal digits.

%href
INT_DIGIT

%group
int limit



%type
User-defined macro
%index
INT_MAX
Maximum value

%inst
A constant that represents the maximum value that can be represented by an integer.

%href
DBL_MAX
INT_MIN

%group
int limit



%type
User-defined macro
%index
INT_MIN
minimum value

%inst
A constant that represents the smallest value that can be represented by an integer.

%href
INT_MAX

%group
int limit





%index
pow
Find power

%prm
(p1, p2)
p1: Bottom (0 or more)
p2: index

%inst
Find the value of p1 raised to the power of p2. The result is given as a real number.

p1 must always be positive. If it is negative, no error will occur, but a non-number (-1. # IND00) will be returned.

p2 can be either positive or negative. You can also specify a real number.

%sample
#include "hspmath.as"
	repeat 5, -2
mes "10" + cnt + "power is" + pow (10, cnt) + "."
	loop
	stop
%href
powf
logf


%index
log10
Logarithm with base 10 (common logarithm)

%prm
(p1)
p1: Antilogarithm

%inst
Find the logarithm of p1 with 10 as the base. The result is given as a real number.

%href
M_LOG10E
logf
log2
%index
log2
Logarithm with base 2

%prm
(p1)
p1: Antilogarithm

%inst
Find the logarithm of p1 with 2 as the base. The result is given as a real number.

%href
M_LOG2E
logf
log10
%index
asin
Inverse function of sine (arc sine)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the arc sine (inverse sine) value of p1.
Results are given in real and radians, in the range -M_PI / 2 to M_PI / 2 (-90 ‘® to 90 ‘® in frequency).

%href
acos
atan

%url
http://ja.wikipedia.org/wiki/%E9%80%86%E4%B8%89%E8%A7%92%E9%96%A2%E6%95%B0
%index
acos
Inverse function of cosine (arc cosine)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the arc cosine (reverse cosine) value of p1.
Results are given in real and radians, ranging from 0 to M_PI (0 ‘® to 180 ‘® in frequency).

%href
asin
atan

%url
http://ja.wikipedia.org/wiki/%E9%80%86%E4%B8%89%E8%A7%92%E9%96%A2%E6%95%B0
%index
sinh
Hyperbolic sine function (hyperbolic sine)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the hyperbolic sine (hyperbolic sine) value of p1.
The result is given as a real number.

%href
cosh
tanh
asinh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
cosh
Hyperbolic cosine function (hyperbolic cosine)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the hyperbolic cosine (hyperbolic cosine) value of p1.
The result is given as a real number.

%href
sinh
tanh
acosh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
tanh
Hyperbolic tangent function (hyperbolic tangent)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the hyperbolic tangent (hyperbolic tangent) value of p1.
The result is given as a real number.

%href
sinh
cosh
atanh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
asinh
Inverse function of hyperbolic sine function (arc hyperbolic sine)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the arc hyperbolic sine (inverse hyperbolic sine) value of p1.
The result is given as a real number.

%href
acosh
atanh
sinh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
acosh
Inverse function of hyperbolic cosine function (arc hyperbolic cosine)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the arc hyperbolic cosine (inverse hyperbolic cosine) value of p1.
The result is given as a real number.

%href
asinh
atanh
cosh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
atanh
Inverse function of hyperbolic tangent function (arc hyperbolic tangent)

%prm
(p1)
p1: Angle value (radian)

%inst
Returns the arc hyperbolic tangent (inverse hyperbolic tangent) value of p1.
The result is given as a real number.

%href
asinh
acosh
tanh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
isfinite
Judgment of finite / infinite / non-number

%prm
(p1)
p1: Judgment value

%inst
Returns 1 if p1 is a finite value, 0 if p1 is infinite or non-number.

Non-numbers do not refer to non-numeric types (strings, labels, modular types, etc.). Use the vartype function to find out the type of a variable.

%sample
#include "hspmath.as"
mes isfinite (10) // finite value
mes isfinite (sqrt (-1)) // non-number
mes isfinite (expf (100)) // finite value
mes isfinite (expf (1000)) // infinity
	stop

%href
isnan
vartype
%index
isnan
Non-numerical judgment

%prm
(p1)
p1: Judgment value

%inst
Returns 1 if p1 is non-number, 0 otherwise.

Non-numbers do not refer to non-numeric types (strings, labels, modular types, etc.). Use the vartype function to find out the type of a variable.

%sample
#include "hspmath.as"
mes isnan (10) // finite value
mes isnan (sqrt (-1)) // non-number
mes isnan (expf (100)) // finite value
mes isnan (expf (1000)) // infinity
	stop

%href
isfinite
vartype

%url
http://ja.wikipedia.org/wiki/%E9%9D%9E%E6%95%B0
%index
round
Rounding

%prm
(p1)
p1: Rounded value (real number)

%inst
Rounds the decimal point of p1 and returns the result as a real number.

%sample
#include "hspmath.as"
	repeat 10
		tmp = 1.0 + 0.1 * cnt
mes strf ("Rounding down% 0.1f", tmp) + strf ("% 0.1f.", round (tmp))
	loop
	stop

%href
intf
floor
ceil

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
sgn
Code

%prm
(p1)
p1: Value to judge the sign (numerical value)

%inst
Determines the sign of a number and returns 1 if it is positive, -1 if it is negative, and 0 if it is zero.

%sample
#include "hspmath.as"
	tmp = 10
	sign = sgn(tmp)
	if sign == 1 {
mes str (tmp) + "is positive."
	} else : if sign == -1 {
mes str (tmp) + "is negative."
	} else {
mes str (tmp) + "is zero."
	}
	stop
%index
intf
Round in the direction of 0

%prm
(p1)
p1: Rounding number

%inst
Returns the number obtained by rounding p1 toward 0. The result is returned as a real number.

Here, "rounding p1 toward 0" means "rounding down the decimal point of p1".

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
Rounding mes strf ("% 4.1f", tmp) + "toward 0 gives" + strf ("% 4.1f", intf (tmp)) + "."
		tmp += 0.5
	wend
	stop

%href
round
floor
ceil

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
floor
Round in the negative direction

%prm
(p1)
p1: Rounding number

%inst
Returns the number obtained by rounding p1 in the negative direction. The result is returned as a real number.

Here, "rounding p1 in the negative direction" means
html{
<ul> <li> Truncate after the decimal point if p1 is positive </ li> <li> Round up after the decimal point if p1 is negative </ ul>
}html
It means that.

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
Rounding mes strf ("% 4.1f", tmp) + "in the negative direction gives" + strf ("% 4.1f", floor (tmp)) + "."
		tmp += 0.5
	wend
	stop

%href
round
intf
ceil

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
ceil
Round in the positive direction

%prm
(p1)
p1: Rounding number

%inst
Returns the number obtained by rounding p1 in the positive direction. The result is returned as a real number.

Here, "rounding p1 in the positive direction" means
html{
<ul> <li> Round up after the decimal point if p1 is positive </ li> <li> Round off after the decimal point if p1 is negative </ ul>
}html
It means that.

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
Rounding mes strf ("% 4.1f", tmp) + "in the positive direction yields" + strf ("% 4.1f", ceil (tmp)) + "."
		tmp += 0.5
	wend
	stop

%href
round
intf
floor

%url
http://ja.wikipedia.org/wiki/%E7%AB%AF%E6%95%B0%E5%87%A6%E7%90%86
%index
fmod
Modulo

%prm
(p1, p2)
p1: Number to be divided
p2: Number to divide

%inst
Returns the remainder when p1 is divided by p2 as a real number.
Unlike the operation with the \\ symbol, it is also valid for real numbers.

%sample
#include "hspmath.as"
	mes 5 \ 2
mes fmod (5, 2) // Return value is real
mes fmod (3.5, 1.2) // The remainder when the real number is divided by the real number is also obtained.
	stop

%url
http://ja.wikipedia.org/wiki/%E3%83%A2%E3%82%B8%E3%83%A5%E3%83%AD
%index
distance2
Find the distance between two points

%prm
(p1, p2)
p1, p2: Numeric array variable with point coordinates assigned

%inst
Find the distance between two points A and B.

Substitute the coordinates of the points in p1 and p2 in the Cartesian coordinate system.
html{
<ul> <li> X coordinate of point A at p1 (0) </ li> <li> Y coordinate of point A at p1 (1) </ li> <li> X coordinate of point B at p2 (0) </ li> <li> Y coordinate of point B at p2 (1) </ li> </ ul>
}html
Please call with the substitution.
The result is returned as a real number.

%sample
#include "hspmath.as"
point_a = 100, 50 // Point A
point_b = 500, 400 // Point B
mes "The distance between two points is" + distance2 (point_a, point_b) + "."
	line point_a(0), point_a(1), point_b(0), point_b(1)
color 255: pset point_a (0), point_a (1) // Point A
color, 255: pset point_b (0), point_b (1) // Point B
	stop
