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
#include "dgMemory.h"
#include "dgVector.h"
#include "dgPlane.h"
#include "dgGoogol.h"
#include "dgIntersections.h"

#define USE_FLOAT_VERSION


#define DG_RAY_TOL_ERROR (dgFloat32 (-1.0e-3f))
#define DG_RAY_TOL_ADAPTIVE_ERROR (dgFloat32 (1.0e-1f))


/*
dgFloat32 dgFastRayTest::PolygonIntersectFallback (const dgVector& normal, dgFloat32 maxT, const dgFloat32* const polygon, dgInt32 strideInBytes, const dgInt32* const indexArray, dgInt32 indexCount) const
{
	dgInt32 stride = dgInt32 (strideInBytes / sizeof (dgFloat32));

	dgBigVector v0 (&polygon[indexArray[indexCount - 1] * stride]);
	dgBigVector m_p0_ (m_p0);
	dgBigVector p0v0_ (v0 - m_p0_);
	dgBigVector normal_ (normal);
	dgBigVector diff_ (m_diff);
	dgFloat64 tOut = normal_.DotProduct3 (p0v0_);
	dgFloat64 dist = normal_.DotProduct3 (diff_);
	if (tOut >= dist * maxT) {
		if ((tOut < dgFloat64 (0.0f)) && (tOut > dist)) {
			for (dgInt32 i = 0; i < indexCount; i ++) {
				dgInt32 i2 = indexArray[i] * stride;
				dgBigVector v1 (&polygon[i2]);
				dgBigVector p0v1_ (v1 - m_p0_);
				// calculate the volume formed by the line and the edge of the polygon
				//dgFloat64 alpha = (diff_ * p0v1_) % p0v0_;
				dgFloat64 alpha = p0v0_.DotProduct3 (diff_.CrossProduct3(p0v1_));
				// if a least one volume is negative it mean the line cross the polygon outside this edge and do not hit the face
				if (alpha < DG_RAY_TOL_ERROR) {
					return 1.2f;
				}
				p0v0_ = p0v1_;
			}

			//the line is to the left of all the polygon edges, 
			//then the intersection is the point we the line intersect the plane of the polygon
			tOut = tOut / dist;
			dgAssert (tOut >= dgFloat32 (0.0f));
			dgAssert (tOut <= dgFloat32 (1.0f));
			return dgFloat32 (tOut);
		}
	}
	return dgFloat32 (1.2f);
}
*/


dgFloat32 dgFastRayTest::PolygonIntersect (const dgVector& normal, dgFloat32 maxT, const dgFloat32* const polygon, dgInt32 strideInBytes, const dgInt32* const indexArray, dgInt32 indexCount) const
{
#if 1
	dgAssert (m_p0.m_w == dgFloat32 (0.0f));
	dgAssert (m_p1.m_w == dgFloat32 (0.0f));
	dgInt32 stride = dgInt32 (strideInBytes / sizeof (dgFloat32));

	dgBigVector v0 (&polygon[indexArray[indexCount - 1] * stride]);
	dgBigVector m_p0_ (m_p0);
	dgBigVector p0v0_ (v0 - m_p0_);
	dgBigVector normal_ (normal);
	dgBigVector diff_ (m_diff);
	dgFloat64 tOut = normal_.DotProduct3 (p0v0_);
	dgFloat64 dist = normal_ .DotProduct3 (diff_);
	if (tOut >= dist * maxT) {
		if ((tOut < dgFloat64 (0.0f)) && (tOut > dist)) {
			for (dgInt32 i = 0; i < indexCount; i ++) {
				dgInt32 i2 = indexArray[i] * stride;
				dgBigVector v1 (&polygon[i2]);
				dgBigVector p0v1_ (v1 - m_p0_);
				// calculate the volume formed by the line and the edge of the polygon
				//dgFloat64 alpha = (diff_ * p0v1_) % p0v0_;
				dgFloat64 alpha = p0v0_.DotProduct3 (diff_.CrossProduct3(p0v1_));
				// if a least one volume is negative it mean the line cross the polygon outside this edge and do not hit the face
				if (alpha < DG_RAY_TOL_ERROR) {
					return 1.2f;
				}
				p0v0_ = p0v1_;
			}

			//the line is to the left of all the polygon edges, 
			//then the intersection is the point we the line intersect the plane of the polygon
			tOut = tOut / dist;
			dgAssert (tOut >= dgFloat32 (0.0f));
			dgAssert (tOut <= dgFloat32 (1.0f));
			return dgFloat32 (tOut);
		}
	}
	return dgFloat32 (1.2f);

#else 
	dgAssert (m_p0.m_w == dgFloat32 (0.0f));
	dgAssert (m_p1.m_w == dgFloat32 (0.0f));

	dgFloat32 dist = normal % m_diff;
	if (dist < m_dirError) {

		dgInt32 stride = dgInt32 (strideInBytes / sizeof (dgFloat32));

		dgVector v0 (&polygon[indexArray[indexCount - 1] * stride]);
		dgVector p0v0 (v0 - m_p0);
		dgFloat32 tOut = normal % p0v0;
		if (tOut >= dist * maxT) {
			// this only work for convex polygons and for single side faces 
			// walk the polygon around the edges and calculate the volume 
			dgFloat32 errorTest = m_magRayTest * DG_RAY_TOL_ADAPTIVE_ERROR;
			if ((tOut < dgFloat32 (0.0f)) && (tOut > dist)) {
				for (dgInt32 i = 0; i < indexCount; i ++) {
					dgInt32 i2 = indexArray[i] * stride;
					dgVector v1 (&polygon[i2]);
					dgVector p0v1 (v1 - m_p0);
					// calculate the volume formed by the line and the edge of the polygon
					dgFloat32 alpha = (m_diff * p0v1) % p0v0;
					// if a least one volume is negative it mean the line cross the polygon outside this edge and do not hit the face
					if (dgAbsf (alpha) < errorTest) {
						return PolygonIntersectFallback (normal, maxT, polygon, strideInBytes, indexArray, indexCount);
					} else if (alpha < 0.0f) {
						return dgFloat32 (1.2f);
					}
					p0v0 = p0v1;
				}

				tOut = tOut / dist;
				dgAssert (tOut >= dgFloat32 (0.0f));
				dgAssert (tOut <= dgFloat32 (1.0f));
				return tOut;
			}
		}
	}
	return dgFloat32 (1.2f);
#endif
}



bool dgApi dgRayBoxClip (dgVector& p0, dgVector& p1, const dgVector& boxP0, const dgVector& boxP1) 
{	
	dgAssert (p0.m_w == dgFloat32(0.0f));
	dgAssert (p1.m_w == dgFloat32(0.0f));
	dgAssert (boxP0.m_w == dgFloat32(0.0f));
	dgAssert (boxP1.m_w == dgFloat32(0.0f));
	for (int i = 0; i < 3; i ++) {
		dgFloat32 tmp0 = boxP1[i] - p0[i];
		if (tmp0 > dgFloat32 (0.0f)) {
			dgFloat32 tmp1 = boxP1[i] - p1[i];
			if (tmp1 < dgFloat32 (0.0f)) {
				p1 = p0 + (p1 - p0).Scale4 (tmp0 / (p1[i] - p0[i])); 
				p1[i] = boxP1[i];
			}
		} else {
			dgFloat32 tmp1 = boxP1[i] - p1[i];
			if (tmp1 > dgFloat32 (0.0f)) {
				p0 += (p1 - p0).Scale4 (tmp0 / (p1[i] - p0[i])); 
				p0[i] = boxP1[i];
			} else {
				return false;
			}
		}

		tmp0 = boxP0[i] - p0[i];
		if (tmp0 < dgFloat32 (0.0f)) {
			dgFloat32 tmp1 = boxP0[i] - p1[i];
			if (tmp1 > dgFloat32 (0.0f)) {
				p1 = p0 + (p1 - p0).Scale4 (tmp0 / (p1[i] - p0[i])); 
				p1[i] = boxP0[i];
			}
		} else {
			dgFloat32 tmp1 = boxP0[i] - p1[i];
			if (tmp1 < dgFloat32 (0.0f)) {
				p0 += (p1 - p0).Scale4 (tmp0 / (p1[i] - p0[i])); 
				p0[i] = boxP0[i];
			} else {
				return false;
			}
		}
	}
	return true;
}


dgBigVector dgPointToRayDistance (const dgBigVector& point, const dgBigVector& ray_p0, const dgBigVector& ray_p1)
{
	dgFloat32 t;
	dgVector dp (ray_p1 - ray_p0);
	t = dgClamp (dp.DotProduct3 (point - ray_p0) / dp.DotProduct3 (dp), dgFloat32(dgFloat32 (0.0f)), dgFloat32 (1.0f));
	return ray_p0 + dp.Scale3 (t);
}

dgBigVector dgPointToTriangleDistance(const dgBigVector& point, const dgBigVector& p0, const dgBigVector& p1, const dgBigVector& p2)
{
	const dgBigVector e10(p1 - p0);
	const dgBigVector e20(p2 - p0);
	const dgFloat64 a00 = e10.DotProduct4(e10).GetScalar();
	const dgFloat64 a11 = e20.DotProduct4(e20).GetScalar();
	const dgFloat64 a01 = e10.DotProduct4(e20).GetScalar();

	const dgFloat64 det = a00 * a11 - a01 * a01;
	dgAssert(det >= dgFloat32(0.0f));
	if (dgAbsf(det) > dgFloat32(1.0e-24f)) {
		dgBigVector p0Point (point - p0);
		const dgFloat64 b0 = e10.DotProduct4(p0Point).GetScalar();
		const dgFloat64 b1 = e20.DotProduct4(p0Point).GetScalar();

		const dgFloat64 beta = b1 * a00 - a01 * b0;
		const dgFloat64 alpha = b0 * a11 - a01 * b1;
		if (beta < dgFloat32(0.0f)) {
			return dgPointToRayDistance (point, p0, p1);
		} else if (alpha < dgFloat32(0.0f)) {
			return dgPointToRayDistance (point, p0, p2);
		} else if ((alpha + beta) > det) {
			return dgPointToRayDistance (point, p1, p2);
		}
		return p0 + (e10.Scale4(alpha) + e20.Scale4(beta)).Scale4(dgFloat64(1.0f) / det);
	}
	// this is a degenerated triangle. this should never happens
	dgAssert(0);
	return p0;
}

dgBigVector dgPointToTetrahedrumDistance (const dgBigVector& point, const dgBigVector& p0, const dgBigVector& p1, const dgBigVector& p2, const dgBigVector& p3)
{
	const dgBigVector e10(p1 - p0);
	const dgBigVector e20(p2 - p0);
	const dgBigVector e30(p3 - p0);

	const dgFloat64 d0 = sqrt(e10.DotProduct4(e10).GetScalar());
	if (d0 > dgFloat64(0.0f)) {
		const dgFloat64 invd0 = dgFloat64(1.0f) / d0;
		const dgFloat64 l10 = e20.DotProduct4(e10).GetScalar() * invd0;
		const dgFloat64 l20 = e30.DotProduct4(e10).GetScalar() * invd0;
		const dgFloat64 desc11 = e20.DotProduct4(e20).GetScalar() - l10 * l10;
		if (desc11 > dgFloat64(0.0f)) {
			const dgFloat64 d1 = sqrt(desc11);
			const dgFloat64 invd1 = dgFloat64(1.0f) / d1;
			const dgFloat64 l21 = (e30.DotProduct4(e20).GetScalar() - l20 * l10) * invd1;
			const dgFloat64 desc22 = e30.DotProduct4(e30).GetScalar() - l20 * l20 - l21 * l21;
			if (desc22 > dgFloat64(0.0f)) {
				dgBigVector p0Point (point - p0);
				const dgFloat64 d2 = sqrt(desc22);
				const dgFloat64 invd2 = dgFloat64(1.0f) / d2;
				
				const dgFloat64 b0 = e10.DotProduct4(p0Point).GetScalar();
				const dgFloat64 b1 = e20.DotProduct4(p0Point).GetScalar();
				const dgFloat64 b2 = e30.DotProduct4(p0Point).GetScalar();

				dgFloat64 u1 = b0 * invd0;
				dgFloat64 u2 = (b1 - l10 * u1) * invd1;
				dgFloat64 u3 = (b2 - l20 * u1 - l21 * u2) * invd2 * invd2;
				u2 = (u2 - l21 * u3) * invd1;
				u1 = (u1 - l10 * u2 - l20 * u3) * invd0;
				if (u3 < dgFloat64(0.0f)) {
					// this looks funny but it is correct
					return dgPointToTriangleDistance(point, p0, p1, p2);
				} else if (u2 < dgFloat64(0.0f)) {
					return dgPointToTriangleDistance(point, p0, p1, p3);
				} else if (u1 < dgFloat64(0.0f)) {
					return dgPointToTriangleDistance(point, p0, p2, p3);
				} else if (u1 + u2 + u3 > dgFloat64(1.0f)) {
					return dgPointToTriangleDistance(point, p1, p2, p3);
				}
				return p0 + e10.Scale4(u1) + e20.Scale4(u2) + e30.Scale4(u3);
			}
		}
	}
	// this is a degenerated tetra. this should never happens
	dgAssert(0);
	return p0;
}

void dgApi dgRayToRayDistance (const dgVector& ray_p0, const dgVector& ray_p1, const dgVector& ray_q0, const dgVector& ray_q1, dgVector& pOut, dgVector& qOut)
{
	dgFloat32 sN;
	dgFloat32 tN;

	dgVector u (ray_p1 - ray_p0);
	dgVector v (ray_q1 - ray_q0);
	dgVector w (ray_p0 - ray_q0);

	dgFloat32 a = u.DotProduct3(u);        
	dgFloat32 b = u.DotProduct3(v);
	dgFloat32 c = v.DotProduct3(v);        
	dgFloat32 d = u.DotProduct3(w);
	dgFloat32 e = v.DotProduct3(w);
	dgFloat32 D = a*c - b*b;   
	dgFloat32 sD = D;			
	dgFloat32 tD = D;			

	// compute the line parameters of the two closest points
	if (D < dgFloat32 (1.0e-8f)) { 
		sN = dgFloat32 (0.0f);        
		sD = dgFloat32 (1.0f);        
		tN = e;
		tD = c;
	} else {                
		// get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < dgFloat32 (0.0f)) {       
			// sc < 0 => the s=0 edge is visible
			sN = dgFloat32 (0.0f);
			tN = e;
			tD = c;
		} else if (sN > sD) {  
			// sc > 1 => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}


	if (tN < dgFloat32 (0.0f)) {           
		// tc < 0 => the t=0 edge is visible
		tN = dgFloat32 (0.0f);
		// recompute sc for this edge
		if (-d < dgFloat32 (0.0f)) {
			sN = dgFloat32 (0.0f);
		} else if (-d > a) {
			sN = sD;
		} else {
			sN = -d;
			sD = a;
		}
	} else if (tN > tD) {      
		// tc > 1 => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < dgFloat32 (0.0f)) {
			sN = dgFloat32 (0.0f);
		} else if ((-d + b) > a) {
			sN = sD;
		} else {
			sN = (-d + b);
			sD = a;
		}
	}

	// finally do the division to get sc and tc
	dgFloat32 sc = (dgAbsf(sN) < dgFloat32(1.0e-8f) ? dgFloat32 (0.0f) : sN / sD);
	dgFloat32 tc = (dgAbsf(tN) < dgFloat32(1.0e-8f) ? dgFloat32 (0.0f) : tN / tD);

	pOut = ray_p0 + u.Scale3 (sc);
	qOut = ray_q0 + v.Scale3 (tc);

/*
	const dgVector p10(ray_p1 - ray_p0);
	const dgVector q10(ray_q1 - ray_q0);
	const dgFloat32 a00 = p10.DotProduct3(p10);
	const dgFloat32 a11 = q10.DotProduct3(q10);
	const dgFloat32 a01 = -(p10.DotProduct3(q10));

	dgFloat32 den = a00 * a11 - a01 * a01;
	dgAssert (den >= dgFloat32 (0.0f));
	if (den > dgFloat32 (1.0e-16f)) {
		const dgVector p0q0(ray_p0 - ray_q0);
		const dgFloat32 b0 = -(p0q0.DotProduct3(p10));
		const dgFloat32 b1 =  (p0q0.DotProduct3(q10));

		dgFloat32 A[2][2];
		dgFloat32 x[2];
		dgFloat32 b[2];
		dgFloat32 l[2];
		dgFloat32 h[2];

		A[0][0] = a00;
		A[0][1] = a01;
		A[1][0] = a01;
		A[1][1] = a11;
		b[0] = b0;
		b[1] = b1;
		x[0] = 0;
		x[1] = 0;
		l[0] = 0.0f;
		l[1] = 0.0f;
		h[0] = 1.0f;
		h[1] = 1.0f;
		dgSolveDantzigLCP(2, &A[0][0], x, b, l, h);
		x[0] *= 1;

		dgFloat32 u = b0 / a00;
		if (u < dgFloat32 (0.0f)) {
			dgAssert (0);
		} else if (u > dgFloat32 (1.0f)) {
			dgAssert (0);
		} else {
			dgAssert (0);
			dgFloat32 du = b0 - a01;
		}


	}
*/

}




dgFloat32 dgRayCastSphere (const dgVector& p0, const dgVector& p1, const dgVector& origin, dgFloat32 radius)
{
	dgVector p0Origin (p0 - origin);
	if (p0Origin.DotProduct3(p0Origin) < (dgFloat32 (100.0f) * radius * radius)) {
		dgVector dp (p1 - p0);
		dgFloat32 a = dp.DotProduct3(dp);
		dgFloat32 b = dgFloat32 (2.0f) * p0Origin.DotProduct3(dp);
		dgFloat32 c = p0Origin.DotProduct3(p0Origin) - radius * radius;
		dgFloat32 desc = b * b - dgFloat32 (4.0f) * a * c;
		if (desc >= 0.0f) {
			desc = dgSqrt (desc);
			dgFloat32 den = dgFloat32 (0.5f) / a;
			dgFloat32 t0 = (-b + desc) * den;
			dgFloat32 t1 = (-b - desc) * den;
			if ((t0 >= dgFloat32 (0.0f)) && (t1 >= dgFloat32 (0.0f))) {
				t0 =  dgMin(t0, t1);
				if (t0 <= dgFloat32 (1.0f)) {
					return t0;
				}
			} else if (t0 >= dgFloat32 (0.0f)) {
				if (t0 <= dgFloat32 (1.0f)) {
					return t0;
				}
			} else {
				if ((t1 >= dgFloat32 (0.0f)) && (t1 <= dgFloat32 (1.0f))) {
					return t1;
				}
			}
		}
	} else {
		dgBigVector p0Origin1 (p0Origin);
		dgBigVector dp (p1 - p0);
		dgFloat64 a = dp.DotProduct3(dp);
		dgFloat64 b = dgFloat32 (2.0f) * p0Origin1.DotProduct3(dp);
		dgFloat64 c = p0Origin1.DotProduct3(p0Origin1) - dgFloat64(radius) * radius;
		dgFloat64 desc = b * b - dgFloat32 (4.0f) * a * c;
		if (desc >= 0.0f) {
			desc = sqrt (desc);
			dgFloat64 den = dgFloat32 (0.5f) / a;
			dgFloat64 t0 = (-b + desc) * den;
			dgFloat64 t1 = (-b - desc) * den;
			if ((t0 >= dgFloat32 (0.0f)) && (t1 >= dgFloat32 (0.0f))) {
				t0 =  dgMin(t0, t1);
				if (t0 <= dgFloat32 (1.0f)) {
					return dgFloat32 (t0);
				}
			} else if (t0 >= dgFloat32 (0.0f)) {
				if (t0 <= dgFloat32 (1.0f)) {
					return dgFloat32 (t0);
				}
			} else {
				if ((t1 >= dgFloat32 (0.0f)) && (t1 <= dgFloat32 (1.0f))) {
					return dgFloat32 (t1);
				}
			}
		}
	}
	return dgFloat32 (1.2f);
}

dgFloat32 dgRayCastBox (const dgVector& p0, const dgVector& p1, const dgVector& boxP0, const dgVector& boxP1, dgVector& normalOut)
{
	dgInt32 index = 0;
	dgFloat32 signDir = dgFloat32 (0.0f);
	dgFloat32 tmin = dgFloat32 (0.0f);
	dgFloat32 tmax = dgFloat32 (1.0f);

	//dgVector size (boxP1 - boxP0);
	for (dgInt32 i = 0; i < 3; i++) {
		dgFloat32 dp = p1[i] - p0[i];
		if (dgAbsf (dp) < dgFloat32 (1.0e-8f)) {
			if (p0[i] <= boxP0[i] || p0[i] >= boxP1[i]) {
				return dgFloat32 (1.2f);
			}
		} else {
			dp = dgFloat32 (1.0f) / dp; 
			dgFloat32 t1 = (boxP0[i] - p0[i]) * dp;
			dgFloat32 t2 = (boxP1[i] - p0[i]) * dp;

			dgFloat32 sign = dgFloat32 (-1.0f);
			if (t1 > t2) {
				sign = 1;
				dgSwap(t1, t2);
			}
			if (t1 > tmin) {
				signDir = sign;
				index = i;
				tmin = t1;
			}
			if (t2 < tmax) {
				tmax = t2;
			}
			if (tmin > tmax) {
				return dgFloat32 (1.2f);
			}
		}
	}

	if (tmin > dgFloat32 (0.0f)) {
		dgAssert (tmin < 1.0f);
		normalOut = dgVector (dgFloat32 (0.0f));
		normalOut[index] = signDir;
	} else {
		tmin = dgFloat32 (1.2f);
	}
	return tmin;

}

