%dll
hspmath
%ver
3.6
%date
2011/05/10
%group
Mathematical Functions
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
User-defined Macro
%index
M_LOG2E
Logarithm of the Napier's number to the base 2

%inst
This constant represents the logarithm of Napier's number to the base 2.
The logarithm of Napier's number to the base 2 refers to log2(m_e).

%group
Mathematical Constants

%sample
#include "hspmath.as"
	a = 10.0
	mes "The logarithm of a to the base 2 is " + log2(a) + "."
	mes "This can also be calculated as logf(a) * m_log2e: " + (logf(a) * m_log2e) + "\n"

	mes "As you can see, by multiplying m_log2e by the \"logarithm to the base e\","
	mes "you can get the \"logarithm to the base 2\"."
	stop

%href
M_LOG10E
M_E
log2

%type
User-defined Macro
%index
M_LOG10E
Logarithm of the Napier's number to the base 10

%inst
This constant represents the logarithm of Napier's number to the base 10.
The logarithm of Napier's number to the base 10 refers to log10(m_e).

%group
Mathematical Constants

%sample
#include "hspmath.as"
	a = m_e
	mes "The logarithm of a to the base 10 is " + log10(a) + "."
	mes "This can also be calculated as logf(a) * m_log10e: " + (logf(a) * m_log10e) + "\n"

	mes "As you can see, by multiplying m_log10e by the \"logarithm to the base e\","
	mes "you can get the \"logarithm to the base 10\"."
	stop

%href
M_LOG2E
log10
M_E


%type
User-defined Macro
%index
M_LN2
Natural logarithm of 2

%inst
This constant represents the natural logarithm of 2.
The natural logarithm of 2 refers to logf(2).

%group
Mathematical Constants

%href
M_LN10



%type
User-defined Macro
%index
M_LN10
Natural logarithm of 10

%inst
This constant represents the natural logarithm of 10.
The natural logarithm of 10 refers to logf(10).

%group
Mathematical Constants

%href
M_LN2


%type
User-defined Macro
%index
M_E
Napier's number

%inst
This constant represents Napier's number (the base of the natural logarithm) e.

%href
M_PI
M_LOG2E
M_LOG10E

%group
Mathematical Constants

%url
http://ja.wikipedia.org/wiki/%E3%83%8D%E3%82%A4%E3%83%94%E3%82%A2%E6%95%B0


%type
User-defined Macro
%index
M_SQRTPI
Square root of pi

%inst
This constant represents the square root of pi.

%href
sqrt
M_PI
M_SQRT2
M_SQRT3

%group
Mathematical Constants


%type
User-defined Macro
%index
M_SQRT2
Square root of 2

%inst
This constant represents the square root of 2.

%href
sqrt
M_SQRTPI
M_SQRT3

%group
Mathematical Constants



%type
User-defined Macro
%index
M_SQRT3
Cube root of 3

%inst
This constant represents the cube root of 3.

%href
sqrt
M_SQRTPI
M_SQRT2

%group
Mathematical Constants


%type
User-defined Macro
%index
DBL_DIG
Number of valid digits in base 10

%inst
This constant represents the number of valid digits in base 10.

%group
Double Limits



%type
User-defined Macro
%index
DBL_EPSILON
Difference between 1.0 and the next largest value

%inst
This constant represents the difference between 1.0 and the next largest value.

%group
Double Limits



%type
User-defined Macro
%index
DBL_MANT_DIG
Number of bits in the mantissa

%inst
This constant represents the number of bits in the mantissa used when representing real numbers.

%group
Double Limits


%type
User-defined Macro
%index
DBL_MAX
Maximum value of a double

%inst
This constant represents the maximum value that can be represented as a double.

%group
Double Limits

%href
INT_MAX



%type
User-defined Macro
%index
DBL_MAX_10_EXP
Maximum exponent value in base 10

%inst
This constant represents the maximum exponent value.

%href
DBL_MIN_10_EXP
DBL_MAX_EXP

%group
Double Limits



%type
User-defined Macro
%index
DBL_MAX_EXP
Maximum exponent value in base 2

%inst
This constant represents the maximum exponent value.

%group
Double Limits

%href
DBL_MAX_10_EXP
INT_MAX


%type
User-defined Macro
%index
DBL_MIN
Smallest value greater than 0

%inst
This constant represents the smallest value greater than 0.

%group
Double Limits



%type
User-defined Macro
%index
DBL_MIN_10_EXP
Minimum exponent value in base 10

%inst
This constant represents the minimum exponent value.

%href
DBL_MAX_10_EXP
DBL_MIN_EXP

%group
Double Limits



%type
User-defined Macro
%index
DBL_MIN_EXP
Minimum exponent value in base 2

%inst
This constant represents the minimum exponent value.

%href
DBL_MIN_10_EXP

%group
Double Limits



%type
User-defined Macro
%index
INT_DIGIT
Number of valid digits in base 2

%inst
This constant represents the number of valid digits in base 2.

%href
INT_DIGIT10

%group
Integer Limits



%type
User-defined Macro
%index
INT_DIGIT10
Number of valid digits in base 10

%inst
This constant represents the number of valid digits in base 10.

%href
INT_DIGIT

%group
Integer Limits



%type
User-defined Macro
%index
INT_MAX
Maximum value

%inst
This constant represents the maximum value that can be represented as an integer.

%href
DBL_MAX
INT_MIN

%group
Integer Limits



%type
User-defined Macro
%index
INT_MIN
Minimum value

%inst
This constant represents the minimum value that can be represented as an integer.

%href
INT_MAX

%group
Integer Limits





%index
pow
Calculate power (exponentiation)

%prm
(p1, p2)
p1 : Base (0 or greater)
p2 : Exponent

%inst
Calculates p1 raised to the power of p2. The result is given as a real number.

p1 must be positive. If it is negative, it will not result in an error, but NaN (-1.#IND00) will be returned.

p2 can be positive or negative. It can also be specified as a real number.

%sample
#include "hspmath.as"
	repeat 5, -2
		mes "10 to the power of " + cnt + " is " + pow(10, cnt) + "."
	loop
	stop
%href
powf
logf


%index
log10
Logarithm to the base 10 (common logarithm)

%prm
(p1)
p1 : Number (argument)

%inst
Calculates the logarithm of p1 to the base 10. The result is given as a real number.

%href
M_LOG10E
logf
log2
%index
log2
Logarithm to the base 2

%prm
(p1)
p1 : Number (argument)

%inst
Calculates the logarithm of p1 to the base 2. The result is given as a real number.

%href
M_LOG2E
logf
log10
%index
asin
Inverse sine function (arcsine)

%prm
(p1)
p1 : Angle value (radians)

%inst
Returns the arcsine (inverse sine) value of p1.
The result is a real number in radians, in the range -M_PI/2 to M_PI/2 (from -90‘® to 90‘® in degrees).

%href
acos
atan

%url
http://ja.wikipedia.org/wiki/%E9%80%86%E4%B8%89%E8%A7%92%E9%96%A2%E6%95%B0
%index
acos
Inverse cosine function (arccosine)

%prm
(p1)
p1 : Angle value (radians)

%inst
Returns the arccosine (inverse cosine) value of p1.
The result is a real number in radians, in the range 0 to M_PI (from 0‘® to 180‘® in degrees).

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
p1 : Angle value (radians)

%inst
Returns the hyperbolic sine value of p1.
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
p1 : Angle value (radians)

%inst
Returns the hyperbolic cosine value of p1.
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
p1 : Angle value (radians)

%inst
Returns the hyperbolic tangent value of p1.
The result is given as a real number.

%href
sinh
cosh
atanh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
asinh
Inverse hyperbolic sine function (arc hyperbolic sine)

%prm
(p1)
p1 : Angle value (radians)

%inst
Returns the arc hyperbolic sine value of p1.
The result is given as a real number.

%href
acosh
atanh
sinh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
acosh
Inverse hyperbolic cosine function (arc hyperbolic cosine)

%prm
(p1)
p1 : Angle value (radians)

%inst
Returns the arc hyperbolic cosine value of p1.
The result is given as a real number.

%href
asinh
atanh
cosh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
atanh
Inverse hyperbolic tangent function (arc hyperbolic tangent)

%prm
(p1)
p1 : Angle value (radians)

%inst
Returns the arc hyperbolic tangent value of p1.
The result is given as a real number.

%href
asinh
acosh
tanh

%url
http://ja.wikipedia.org/wiki/%E5%8F%8C%E6%9B%B2%E7%B7%9A%E9%96%A2%E6%95%B0
%index
isfinite
Finite/Infinite/NaN determination

%prm
(p1)
p1 : Value to determine

%inst
Returns 1 if p1 is a finite value, and 0 if p1 is infinite or NaN (Not a Number).

NaN does not refer to types other than numbers (strings, labels, module types, etc.). Use the vartype function to check the type of a variable.

%sample
#include "hspmath.as"
	mes isfinite(10)            // Finite value
	mes isfinite(sqrt(-1))      // NaN
	mes isfinite(expf(100))     // Finite value
	mes isfinite(expf(1000))    // Infinity
	stop

%href
isnan
vartype
%index
isnan
NaN determination

%prm
(p1)
p1 : Value to determine

%inst
Returns 1 if p1 is NaN (Not a Number), and 0 otherwise.

NaN does not refer to types other than numbers (strings, labels, module types, etc.). Use the vartype function to check the type of a variable.

%sample
#include "hspmath.as"
	mes isnan(10)               // Finite value
	mes isnan(sqrt(-1))         // NaN
	mes isnan(expf(100))        // Finite value
	mes isnan(expf(1000))       // Infinity
	stop

%href
isfinite
vartype

%url
http://ja.wikipedia.org/wiki/%E9%9D%9E%E6%95%B0
%index
round
Round to the nearest integer

%prm
(p1)
p1 : Value to round (real number)

%inst
Rounds p1 to the nearest integer and returns the result as a real number.

%sample
#include "hspmath.as"
	repeat 10
		tmp = 1.0 + 0.1 * cnt
		mes strf("Rounding %0.1f gives ", tmp) + strf("%0.1f.", round(tmp))
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
Sign

%prm
(p1)
p1 : Value to determine the sign (number)

%inst
Determines the sign of a number and returns 1 if it is positive, -1 if it is negative, and 0 if it is zero.

%sample
#include "hspmath.as"
	tmp = 10
	sign = sgn(tmp)
	if sign == 1 {
		mes str(tmp) + " is positive."
	} else : if sign == -1 {
		mes str(tmp) + " is negative."
	} else {
		mes str(tmp) + " is zero."
	}
	stop
%index
intf
Round toward zero

%prm
(p1)
p1 : Numeric value to round

%inst
Returns p1 rounded toward zero. The result is returned as a real number.
Here, "rounding p1 towards 0" means "truncating the decimal part of p1".

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
		mes strf("%4.1f", tmp) + " rounded towards 0 is, " + strf("%4.1f", intf(tmp)) + "."
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
Rounding towards negative infinity

%prm
(p1)
p1 : The number to round

%inst
Returns the value of p1 rounded towards negative infinity. The result is returned as a real number.

Here, "rounding p1 towards negative infinity" means:
html{
<ul><li>Truncate the decimal part if p1 is positive</li><li>Round up the decimal part if p1 is negative</ul>
}html

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
		mes strf("%4.1f", tmp) + " rounded towards negative infinity is, " + strf("%4.1f", floor(tmp)) + "."
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
Rounding towards positive infinity

%prm
(p1)
p1 : The number to round

%inst
Returns the value of p1 rounded towards positive infinity. The result is returned as a real number.

Here, "rounding p1 towards positive infinity" means:
html{
<ul><li>Round up the decimal part if p1 is positive</li><li>Truncate the decimal part if p1 is negative</ul>
}html

%sample
#include "hspmath.as"
	tmp = -2.5
	while(tmp <= 2.5)
		mes strf("%4.1f", tmp) + " rounded towards positive infinity is, " + strf("%4.1f", ceil(tmp)) + "."
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
p1 : The number to be divided
p2 : The divisor

%inst
Returns the remainder when p1 is divided by p2 as a real number.
Unlike the \ operator, this is valid for real numbers.

%sample
#include "hspmath.as"
	mes 5 \ 2
	mes fmod(5, 2)      // Returns a real number
	mes fmod(3.5, 1.2)  // You can also find the remainder when a real number is divided by a real number
	stop

%url
http://ja.wikipedia.org/wiki/%E3%83%A2%E3%82%B8%E3%83%A5%E3%83%AD
%index
distance2
Calculates the distance between two points

%prm
(p1, p2)
p1, p2 : Numerical array variables storing the coordinates of the points

%inst
Calculates the distance between two points A and B.

The coordinates of the points should be assigned to p1 and p2 in a Cartesian coordinate system beforehand.
html{
<ul><li>p1(0) is the X coordinate of point A</li><li>p1(1) is the Y coordinate of point A</li><li>p2(0) is the X coordinate of point B</li><li>p2(1) is the Y coordinate of point B</li></ul>
}html
Should be called with the above assigned.
The result is returned as a real number.

%sample
#include "hspmath.as"
	point_a = 100, 50       // Point A
	point_b = 500, 400      // Point B
	mes "The distance between the two points is " + distance2(point_a, point_b) + "."
	line point_a(0), point_a(1), point_b(0), point_b(1)
	color 255  : pset point_a(0), point_a(1)    // Point A
	color ,255 : pset point_b(0), point_b(1)    // Point B
	stop
