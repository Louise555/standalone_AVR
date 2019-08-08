                                          /* Scale in the key of 1/200 */

/*
   These are periods -- if you delay this long,
   then toggle the speaker pin, you'll get approximate
   pitches.

   This is the 8-bit version.  The pitches get less accurate
   as they get higher.
https://pages.mtu.edu/~suits/notefreqs.html
so we need a time between clock of 64us . so for a 16MHz clock we divide by 1028 . the frequency is 15564 Hz
so time between tick = 64.25 us. so if the counter stops when its value is 238 (A0) that means 64.25us x 239 = 15.3ms 
ot a period of 15.3ms x 2= 30.6 Hz. It is about A0 but approximation. 

*/

#define Gx0 252
#define A0  238
#define Ax0 224
#define B0  212
#define C1  200
#define Cx0 189
#define D1  178
#define Dx0 168
#define E1  159
#define F1  150
#define Fx1 141
#define G1  133
#define Gx1 126
#define A1  119
#define Ax1 112
#define B1  106
#define C2  100
#define Cx2 94
#define D2  89
#define Dx2 84
#define E2  79
#define F2  75
#define Fx2 71
#define G2  67
#define Gx2 63
#define A2  59
#define Ax2 56
#define B2  53
#define C3  50
#define Cx3 47
#define D3  44
#define Dx3 42
#define E3  40
#define F3  37
#define Fx3 35
#define G3  33
