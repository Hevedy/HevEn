/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "dgStdafx.h"
#include "dgGeneralMatrix.h"
#include "dgStack.h"
#include "dgMemory.h"


/*
class TestSolver_xxxxxxx: public SymmetricBiconjugateGradientSolve
{
	public:
	dgFloat64 a[4][4];

	TestSolver_xxxxxxx()
		:SymmetricBiconjugateGradientSolve()
	{
		dgFloat64 b[] = {1, 2, 3, 4};
		dgFloat64 x[] = {0, 0, 0, 0};
		dgFloat64 c[4];

		memset (a, 0, sizeof (a));
		a[0][0] = 2;
		a[1][1] = 3;
		a[2][2] = 4;
		a[0][3] = 1;
		a[1][3] = 1;
		a[2][3] = 1;
		a[3][0] = 1;
		a[3][1] = 1;
		a[3][2] = 1;


		Solve (4, dgFloat64  (1.0e-10f), x, b);

		MatrixTimeVector (c, x);
		MatrixTimeVector (c, x);
	}

	void MatrixTimeVector (dgFloat64* const out, const dgFloat64* const v) const
	{
		out[0] = a[0][0] * v[0] + a[0][1] * v[1] + a[0][2] * v[2] + a[0][3] * v[3];
		out[1] = a[1][0] * v[0] + a[1][1] * v[1] + a[1][2] * v[2] + a[1][3] * v[3];
		out[2] = a[2][0] * v[0] + a[2][1] * v[1] + a[2][2] * v[2] + a[2][3] * v[3];
		out[3] = a[3][0] * v[0] + a[3][1] * v[1] + a[3][2] * v[2] + a[3][3] * v[3];
	}

	void InversePrecoditionerTimeVector (dgFloat64* const out, const dgFloat64* const v) const
	{
		out[0] = v[0]/a[0][0];
		out[1] = v[1]/a[1][1];
		out[2] = v[2]/a[2][2];
		out[3] = v[3];
	}
};
*/


dgSymmetricBiconjugateGradientSolve::dgSymmetricBiconjugateGradientSolve ()
{
}

dgSymmetricBiconjugateGradientSolve::~dgSymmetricBiconjugateGradientSolve ()
{
}


void dgSymmetricBiconjugateGradientSolve::ScaleAdd (dgInt32 size, dgFloat64* const a, const dgFloat64* const b, dgFloat64 scale, const dgFloat64* const c) const
{
	for (dgInt32 i = 0; i < size; i ++) {
		a[i] = b[i] + scale * c[i];
	}
}

void dgSymmetricBiconjugateGradientSolve::Sub (dgInt32 size, dgFloat64* const a, const dgFloat64* const b, const dgFloat64* const c) const
{
	for (dgInt32 i = 0; i < size; i ++) {
		a[i] = b[i] - c[i];
	}
}

dgFloat64 dgSymmetricBiconjugateGradientSolve::DotProduct (dgInt32 size, const dgFloat64* const b, const dgFloat64* const c) const
{
	dgFloat64 product = dgFloat64 (0.0f);
	for (dgInt32 i = 0; i < size; i ++) {
		product += b[i] * c[i];
	}
	return product;
}

dgFloat64 dgSymmetricBiconjugateGradientSolve::Solve (dgInt32 size, dgFloat64 tolerance, dgFloat64* const x, const dgFloat64* const b) const
{
	dgStack<dgFloat64> bufferR0(size);
	dgStack<dgFloat64> bufferP0(size);
	dgStack<dgFloat64> matrixTimesP0(size);
	dgStack<dgFloat64> bufferConditionerInverseTimesR0(size);

	dgFloat64* const r0 = &bufferR0[0];
	dgFloat64* const p0 = &bufferP0[0];
	dgFloat64* const MinvR0 = &bufferConditionerInverseTimesR0[0];
	dgFloat64* const matrixP0 = &matrixTimesP0[0];

	MatrixTimeVector (matrixP0, x);
	Sub(size, r0, b, matrixP0);
	bool continueExecution = InversePrecoditionerTimeVector (p0, r0);

	dgInt32 iter = 0;
	dgFloat64 num = DotProduct (size, r0, p0);
	dgFloat64 error2 = num;
	for (dgInt32 j = 0; (j < size) && (error2 > tolerance) && continueExecution; j ++) {

		MatrixTimeVector (matrixP0, p0);
		dgFloat64 den = DotProduct (size, p0, matrixP0);

		dgAssert (fabs(den) > dgFloat64 (0.0f));
		dgFloat64 alpha = num / den;

		ScaleAdd (size, x, x, alpha, p0);
        if ((j % 50) != 49) {
		    ScaleAdd (size, r0, r0, -alpha, matrixP0);
        } else {
            MatrixTimeVector (matrixP0, x);
            Sub(size, r0, b, matrixP0);
        }

//dgUnsigned64 xxx0 = dgGetTimeInMicrosenconds();
		continueExecution = InversePrecoditionerTimeVector (MinvR0, r0);
//xxx0 = dgGetTimeInMicrosenconds() - xxx0;
//dgTrace (("%d\n", dgUnsigned64 (xxx0)));


		dgFloat64 num1 = DotProduct (size, r0, MinvR0);
		dgFloat64 beta = num1 / num;
		ScaleAdd (size, p0, MinvR0, beta, p0);
		num = DotProduct (size, r0, MinvR0);
		iter ++;
		error2 = num;
		if (j > 10) {
			error2 = dgFloat64 (0.0f);
			for (dgInt32 i = 0; i < size; i ++) {
				error2 = dgMax (error2, r0[i] * r0[i]);
			}
		}
	}

	dgAssert (iter < size);
	return num;
}
