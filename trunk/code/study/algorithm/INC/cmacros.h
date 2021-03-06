/**********************************************************************************************************************
*	$Log: vsys#comm#inc#cmacros.h,v $
*	Revision 1.5  2007-11-20 17:11:46-08  lsha
*	remove bSETMASK warning
*
*	Revision 1.4  2007-11-15 21:37:08-08  lsha
*	...No comments entered during checkin...
*
*	Revision 1.1  2007-08-27 18:52:10-07  lsha
*	Initial revision.
*
*
*	DESCRIPTION:
*	It includes useful functional macro definitions.
*
**********************************************************************************************************************/

#ifndef	CMACROS
#define	CMACROS						"         CMACROS >>>    "
/**	CMACROS
 */



#ifdef	__cplusplus
	extern	"C"
	{
#endif

/**	SECTION - symbols
 */
	#ifndef	Global
		#ifdef	__MAIN__
		#define	Global
		#define	GlobalAssign(g, x)	g = (x)

		#else
		#define	Global				extern
		#define	GlobalAssign(g, x)	g
		#endif
	#endif

	#ifdef	CONST
	#undef	CONST
	#endif
		#define	CONST				static const

/**	ENDOFSECTION
 */



/**	SECTION - unit constants for storage, frequency and bit-rate
 */
	#ifndef	INFINITY32
		#define	KILO				(1024)
		#define	kilo				(1000)

		#define	MEGA				(1024  * 1024)
		#define	mega				(1000  * 1000)

		#define	GIGA				(1024. * 1024. * 1024.)
		#define	giga				(1000. * 1000. * 1000.)

		#define	TERA				(1024. * 1024. * 1024. * 1024.)
		#define	tera				(1000. * 1000. * 1000. * 1000.)

		#define	KILOth				(1. / KILO)
		#define	kiloth				(1. / kilo)

		#define	MEGAth				(1. / MEGA)
		#define	megath				(1. / mega)

		#define	GIGAth				(1. / GIGA)
		#define	gigath				(1. / giga)

		#define	TERAth				(1. / TERA)
		#define	terath				(1. / tera)

		#define	INFINITY32			(1 << 28)			/* Help to avoid 32b operation over-flow */
	#endif

/**	ENDOFSECTION
 */



/**	SECTION - macros for basic math operations
 */
	#ifdef	MIN
	#undef	MIN
	#endif
		#define	MIN(a, b)					((a) < (b) ? (a) : (b))

	#ifdef	MAX
	#undef	MAX
	#endif
		#define	MAX(a, b)					((a) > (b) ? (a) : (b))

	#ifdef	ABS
	#undef	ABS
	#endif
		#define	ABS(x)						((x) < 0 ? - (x) : (x))

	#ifdef	INLINE
		INLINE SIGN32 ABS32(SIGN32 x)		{ SIGN32 s = x >> 31; return (x ^ s) - s; }
	#endif

	#ifdef	SIGN
	#undef	SIGN
	#endif
		#define	SIGN(x)						((x) < 0 ? - 1 : 1)

	#ifdef	ROUND
	#undef	ROUND
	#endif
		#define	ROUND(x)					((SIGN32)floor((x) + 0.5))

		#define	SCLb(k)						(31 & (k))
		#define	UNIVSCL(a, k)				(((k) < 0) ? ((a) >> SCLb(- (k))) : ((a) << SCLb(k)))
		#define	UNSGDES(a, k)				(((a) + (1 << SCLb(k) >> 1)) >> SCLb(k))
		#define	UNSGSCL(a, k)				(((k) < 0) ? UNSGDES(a, - (k)) : ((a) << SCLb(k)))
		#define	SIGNDES(a, k)				(((a) > 0) ? - UNSGDES(- (a), k) : UNSGDES(a, k))
		#define	SIGNSCL(a, k)				(((k) < 0) ? SIGNDES(a, - (k)) : ((a) << SCLb(k)))
		#define	REALSCL(a, k)				(((k) < 0) ? (a) / (1 << SCLb(- (k))) : (a) * (1 << SCLb(k)))
		#define	UNIVRND(k)					UNIVSCL(1, (k) - 1)

		#define	EVENDIV(x)					(((x) >> 1) | ((x) & 1))
		#define	CEILDIV(a, b)				(((a) + (b) - 1) / (b))
		#define	UNSGDIV(a, b)				(((a) + ((b) >> 1)) / (b))
		#define	SIGNDIV(a, b)				((((a) > 0) ? ((a) + ((b) >> 1)) : ((a) - ((b) >> 1))) / (b))
		#define	NSclDIV(a, k1, b, k2)		((a) * (k1) / (b) * (k2) + UNSGDIV((((a) * (k1)) % (b)) * (k2), b))

		#define	Average256(x1, x2, a)		(((x1) * (a) + (x2) * (256 - (a)) + 128) >> 8)
		#define	Average(x1, x2, a, s)		(((x1) * (a) + (x2) * ((s) - (a)) + ((s) >> 1)) / (s))
		#define	Average3(r, x1, p1, x2, p2, x3, p3)																	\
											(((r) + (x1) * (p1) + (x2) * (p2) + (x3) * (p3)) / ((p1) + (p2) + (p3)))
		#define	Median(a, b, c)				((a) > (b) ? ((b) > (c) ? (b) : MIN(a, c)) : ((b) < (c) ? (b) : MAX(a, c)))

		#define	SATURATE(x, min, max)		MAX(MIN((x), (max)), (min))
		#define	UNIVCLP(x, lmt)				(((lmt) < 0) ? SATURATE(x, lmt, - (lmt) - 1) :							\
											(((lmt) > 0) ? SATURATE(x, 0, lmt) :									\
																(x)))
		#define	ModInc(x, i, mod)			do{	(x) += (i); while((x) >= (mod)) (x) -= (mod); }while(0)
		#define	RangeBits(x, b)				do{	for((b) = 0; ; (b) ++)												\
													if((UNSG32)(x) < (UNSG32)(1 << (b))) break;						\
																}while(0)
/**	ENDOFSECTION
 */



/**	SECTION - macros for bits operations
 */
		#define	bTST(x, b)					(((x) >> (b)) & 1)
		#define	bSETMASK(b)					((b) < 32 ? (1 << ((b) & 31)) : 0)
		#define	bSET(x, b)					do{	(x) |= bSETMASK(b); }while(0)
		#define	bCLRMASK(b)					(~(bSETMASK(b)))
		#define	bCLR(x, b)					do{	(x) &= bCLRMASK(b); }while(0)
		#define	NSETMASK(msb, lsb)			(bSETMASK((msb) + 1) - bSETMASK(lsb))
		#define	NCLRMASK(msb, lsb)			(~(NSETMASK(msb, lsb)))
		#define	CutTo(x, b)					((x) & (bSETMASK(b) - 1))
		#define	SignedRestore(x, b)			((SIGN32)(x) << (32 - (b)) >> (32 - (b)))
		#define	SignMagnitude(x, b)			CutTo((x) < 0 ? bSETMASK((b) - 1) - (x) : (x), b)
		#define	InvSignMagnitude(x, b)		((SIGN32)(bTST(x, (b) - 1) ? bSETMASK((b) - 1) - (x) : (x)))
		#define	ClpSignMagnitude(r, x, b)	do{	SIGN32 lmt = (1 << ((b) - 1)) - 1, y = SATURATE(x, - lmt, lmt);		\
												r = SignMagnitude(y, b);											\
																}while(0)
		#define	GetField(r, b, mask)		(((r) & (mask)) >> (b))
		#define	BFGET(r, lsb, b)			GetField(r, lsb, NSETMASK((lsb) + (b) - 1, lsb))
		#define	vreg_GetField(r, name)		GetField(r, CB_##name, CM_##name)
		#define	SetField(r, b, mask, f)		do{	(r) &= ~(mask); (r) |= ((f) << (b)) & (mask); }while(0)
		#define	BFSET(r, lsb, b, f)			SetField(r, lsb, NSETMASK((lsb) + (b) - 1, lsb), f)
		#define	vreg_SetField(r, name, f)	SetField(r, CB_##name, CM_##name, f)
		#define	vreg_Init(a, v, reg, rst, base)			do{	(a) = (base) + RA_##reg; (v) = (rst); }while(0)

		#define	GetUnsigned(bf, v, b)		do{	(v) = (UNSG32)(bf); }while(0)
		#define	GetSigned(bf, v, b)			do{	(v) = SignedRestore((UNSG32)bf, b); }while(0)
		#define	GetSignMagnitude(bf, v, b)	do{	(v) = InvSignMagnitude((UNSG32)bf, b); }while(0)
		#define	SetUnsigned(bf, v, b)		do{	(bf) = CutTo((UNSG32)v, b); }while(0)
		#define	SetSigned(bf, v, b)			SetUnsigned(bf, v, b)
		#define	SetSignMagnitude(bf, v, b)	do{	(bf) = SignMagnitude((SIGN32)v, b); }while(0)

		#define	UnsignedBF(v, b, lsb)		(CutTo((UNSG32)v, b) << (lsb))
		#define	SignedBF(v, b, lsb)			(CutTo((SIGN32)v, b) << (lsb))
		#define	SignMagnitudeBF(v, b, lsb)	(SignMagnitude((SIGN32)v, b) << (lsb))

		#define	sizeofType(Ta, Tb)			(sizeof(Ta) / sizeof(Tb))
		#define	sizeofArray(arr)			(sizeofType(arr, arr[0]))
		#define	endofString(str, i)			((str) + (strlen(str) - i))

	#ifdef	__big_endian__
		#define	MSi8						(0x0)
		#define	LSi8						(0x3)
		#define	MSi16						(0x0)
		#define	LSi16						(0x1)
		#define	MSB32(u32)					(((UNSG8*)&(u32))[0])
		#define	LSB32(u32)					(((UNSG8*)&(u32))[3])
	#else
		#define	MSi8						(0x3)
		#define	LSi8						(0x0)
		#define	MSi16						(0x1)
		#define	LSi16						(0x0)
		#define	MSB32(u32)					(((UNSG8*)&(u32))[3])
		#define	LSB32(u32)					(((UNSG8*)&(u32))[0])
	#endif

/**	ENDOFSECTION
 */



/**	SECTION - macros for miscellaneous functional shortcuts
 */
	/* FIFO level decrease by N, return SUCCESS or ERR_FIFO if under-flow */
		#define	FiFoDec(lvl, n)				(((lvl) < (n)) ? ERR_FIFO : ((lvl) -= (n), SUCCESS))

	/* FIFO level increase by N, return SUCCESS or ERR_FIFO if over-flow */
		#define	FiFoInc(lvl, n, max)		(((lvl) + (n) > (max)) ? ERR_FIFO : ((lvl) += (n), SUCCESS))

	/* Parse function arguments */
		#define	LET(x, arg, dftval)			do{	if((arg) != ARG_KEEP) x = ((arg) == ARG_AUTO) ? (dftval) : (arg);	\
																}while(0)
	/* Copy an array */
		#define	CpyN(n, T, tgt, src)		do{	SIGN32 ni;															\
												for(ni = 0; ni < (n); ni ++) (tgt)[ni] = (T)((src)[ni]);			\
																}while(0)
	/* Clip-copy an array */
		#define	ClpN(n, tgt, src, min, max)	do{	SIGN32 ni;															\
												for(ni = 0; ni < (n); ni ++)										\
													(tgt)[ni] = SATURATE((src)[ni], min, max);						\
																}while(0)
	/* Optimize memory pointer (default: pow2 = 32) */
		#define	MemAlign(pmem, pow2)		(((UNSG8*)(pmem)) + ((pow2) - 1) - ((ptr32u(pmem) - 1) & ((pow2) - 1)))

	/* Copy 2 32b pointers (non-burst mode) */
		#define	RegCpy(pa, pb, regs)		do{	SIGN32 ni, cnt = (regs);											\
												for(ni = 0; ni < cnt; ni ++) ((UNSG32*)pa)[ni] = ((UNSG32*)pb)[ni];	\
																}while(0)
	/* Copy 2 pointers of any same structure type */
		#define	EleCpy(pa, pb, type)		RegCpy(pa, pb, sizeof(type) >> 2)

	/* Print a number to binary bits */
		#define	ToBinary(x, b, str)			do{	SIGN32 ni;															\
												for(ni = 0; ni < (b); ni ++)										\
													(str)[ni] = bTST(x, (b) - ni - 1) ? '1' : '0';					\
												(str)[ni] = 0;														\
																}while(0)
	/* Scan a number from binary bits */
		#define	FromBinary(x, b, str)		do{	SIGN32 ni; (x) = 0;													\
												for(ni = 0; ni < (b); ni ++)										\
													(x) = ((x) << 1) | (((str)[ni] == '1') ? 1 : 0);				\
																}while(0)
/**	ENDOFSECTION
 */

#ifdef	__cplusplus
	}
#endif



/**	CMACROS
 */
#endif

/**	ENDOFFILE: cmacros.h **********************************************************************************************
 */

