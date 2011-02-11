/*
 *  Numerics/Quaternion.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/07/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Quaternion.h"

namespace Dream
{
	namespace Numerics
	{
		template <typename _NumericT>
		Quaternion<_NumericT> Quaternion<_NumericT>::sphericalLinearInterpolate (_NumericT t, const Quaternion<_NumericT> & q0,
																				   const Quaternion<_NumericT> & q1)
		{
			// v0 and v1 should be unit length or else
			// something broken will happen.

			const Vec4T & v0 = q0.vector();
			const Vec4T & v1 = q1.vector();

			// Compute the cosine of the angle between the two vectors.
			NumericT dot = v0.dot(v1);

			const RealT DOT_THRESHOLD = 0.9995;
			if (dot > DOT_THRESHOLD)
			{
				// If the inputs are too close for comfort, linearly interpolate
				// and normalize the result.
				Vec4T result(v0 + (v1 - (Vec4)v0) * t);

				result.normalize();
				return Quaternion(result);
			}

			// Robustness: Stay within domain of acos():
			Number<NumericT>::clamp(dot);
			// theta_0 = angle between input vectors:
			RealT theta_0 = acos(dot);
			// theta = angle between v0 and result:
			RealT theta = theta_0 * t;
			Vec4T v2 = v1 - v0 * dot;

			v2.normalize();
			// { v0, v2 } is now an orthonormal basis.

			return Quaternion(v0*cos(theta) + v2*sin(theta));
		}

		template <typename _NumericT>
		Quaternion<_NumericT>::Quaternion (const Identity &)
		{
			m_vector.zero();
			m_vector[W] = 1;
		}

		template <typename _NumericT>
		Quaternion<_NumericT>::Quaternion (const Vec4T & other)
		{
			m_vector.set(other);
		}

		template <typename _NumericT>
		Quaternion<_NumericT>::Quaternion (const _NumericT & angle, const Vec3T & axis)
		{
			setToAngleAxisRotation(angle, axis);
		}

		template <typename _NumericT>
		Quaternion<_NumericT>::Quaternion (const Vec3T & point)
		{
			m_vector.set(point);
			m_vector[W] = 0;
		}

		template <typename _NumericT>
		Quaternion<_NumericT>::Quaternion (const Vec3T & from, const Vec3T & to, _NumericT factor)
		{
			Vec3T axis = from.cross(to).normalize();
			RealT angle = from.angleBetween(to) * factor;

			setToAngleAxisRotation(angle, axis);
		}
		
		// Rotation from A to B
		template <typename _NumericT>
		Quaternion<_NumericT>::Quaternion (const Vec3T & from, const Vec3T & to)
		{
			Vec3T axis = from.cross(to).normalize();
			RealT angle = from.angleBetween(to);

			setToAngleAxisRotation(angle, axis);
		}
		
		template <typename _NumericT>
		Quaternion<_NumericT> Quaternion<_NumericT>::fromMatrix (const Mat44 & m) {
			RealT w = Math::sqrt(1 + m.at(0) + m.at(5) + m.at(10)) / 2.0;

			Vec4 e;						
			e[X] = (m.at(6) - m.at(9)) / (4 * w);
			e[Y] = (m.at(8) - m.at(2)) / (4 * w);
			e[Z] = (m.at(1) - m.at(4)) / (4 * w);
			e[W] = w;

			return Quaternion(e.normalizedVector(1));
		}

		template <typename _NumericT>
		const _NumericT & Quaternion<_NumericT>::operator[] (unsigned i) const
		{
			return m_vector[i];
		}

		template <typename _NumericT>
		_NumericT & Quaternion<_NumericT>::operator[] (unsigned i)
		{
			return m_vector[i];
		}

		template <typename _NumericT>
		typename Quaternion<_NumericT>::Vec3T Quaternion<_NumericT>::rotate (const Vec3T & pt) const
		{
			Quaternion conj = this->conjugatedQuaternion();
			Quaternion qpt(pt);

			Quaternion r1 = (*this) * qpt;
			Quaternion r2 = r1 * conj;

			// Remove w component
			return r2.m_vector.reduce();
		}

		template <typename _NumericT>
		inline typename Quaternion<_NumericT>::Vec3T Quaternion<_NumericT>::operator* (const Vec3T & other) const
		{
			return rotate(other);
		}

		template <typename _NumericT>
		Quaternion<_NumericT> Quaternion<_NumericT>::multiplyWith (const Quaternion<_NumericT> & q1) const
		{
			const Quaternion & q2 = *this;

			Quaternion result;

			result[X] = (q1[W]*q2[X] + q1[X]*q2[W] + q1[Y]*q2[Z]) - q1[Z]*q2[Y];
			result[Y] = (q1[W]*q2[Y] + q1[Y]*q2[W] + q1[Z]*q2[X]) - q1[X]*q2[Z];
			result[Z] = (q1[W]*q2[Z] + q1[X]*q2[Y] + q1[Z]*q2[W]) - q1[Y]*q2[X];
			result[W] = q1[W]*q2[W] - q1[X]*q2[X] - q1[Y]*q2[Y] - q1[Z]*q2[Z];

			return result;
		}

		template <typename _NumericT>
		Quaternion<_NumericT> & Quaternion<_NumericT>::operator*= (const Quaternion & q2)
		{
			Quaternion r = multiplyWith(q2);
			m_vector = r.m_vector;

			return *this;
		}

		template <typename _NumericT>
		Quaternion<_NumericT> Quaternion<_NumericT>::operator* (const Quaternion & q2) const
		{
			return multiplyWith(q2);
		}

		template <typename _NumericT>
		Quaternion<_NumericT> & Quaternion<_NumericT>::operator= (const Vec4T & other)
		{
			m_vector.set(other);

			return *this;
		}

		template <typename _NumericT>
		typename Quaternion<_NumericT>::Vec3T Quaternion<_NumericT>::rotationAxis () const
		{
			return m_vector.reduce().normalize();
		}

		template <typename _NumericT>
		typename Quaternion<_NumericT>::MatrixT Quaternion<_NumericT>::rotatingMatrix () const
		{
			Matrix<4, 4, NumericT> matrix(IDENTITY);

			ensure (Numerics::equalWithinTolerance(m_vector.length2(), (NumericT)1.0)
					&& "Quaternion.rotatingMatrix magnitude must be 1");

			NumericT x = m_vector[X], y = m_vector[Y], z = m_vector[Z], w = m_vector[W];

			matrix.at(0, 0) = 1 - 2 * (y*y + z*z);
			matrix.at(1, 0) =	  2 * (x*y - w*z);
			matrix.at(2, 0) =     2 * (x*z + w*y);

			matrix.at(0, 1) =     2 * (x*y + w*z);
			matrix.at(1, 1) = 1 - 2 * (x*x + z*z);
			matrix.at(2, 1) =     2 * (y*z - w*x);

			matrix.at(0, 2) =     2 * (x*z - w*y);
			matrix.at(1, 2) =     2 * (y*z + w*x);
			matrix.at(2, 2) = 1 - 2 * (x*x + y*y);
			
			return matrix;
		}
		
		template <typename _NumericT>
		typename Quaternion<_NumericT>::Vec3T Quaternion<_NumericT>::extractAxis (IndexT a) const
		{
			Vec3T result;
			
			if (a == X) {
				result = Vec3(1, 0, 0);
			} else if (a == Y) {
				result = Vec3(0, 1, 0);
			} else if (a == Z) {
				result = Vec3(0, 0, 1);
			}
			
			return rotatingMatrix() * result;
		}

#ifdef OPENGL_SUPPORT
		template <typename _NumericT>
		void Quaternion<_NumericT>::glRotate () const
		{
			Vec3T axis = rotationAxis().normalize();

			//std::cout << "Rotation angle: " << rotationAngle() << " axis: " << axis << std::endl;
			glRotatef(rotationAngle() * RADIANS_TO_DEGREES, axis[X], axis[Y], axis[Z]);
		}
#endif

		template <typename _NumericT>
		void Quaternion<_NumericT>::setToAngleAxisRotation (const RealT & angle, const Vec3T & axis)
		{
			m_vector.set(axis);
			m_vector *= Math::sin(angle / 2.0);

			m_vector[W] = Math::cos(angle / 2.0);
		}

		template <typename _NumericT>
		_NumericT Quaternion<_NumericT>::rotationAngle () const
		{
			return Number<NumericT>::acos(m_vector[W]) * 2;
		}

		template <typename _NumericT>
		void Quaternion<_NumericT>::conjugate ()
		{
			m_vector[X] *= -1;
			m_vector[Y] *= -1;
			m_vector[Z] *= -1;
		}

		template <typename _NumericT>
		Quaternion<_NumericT> Quaternion<_NumericT>::conjugatedQuaternion () const
		{
			Quaternion result(*this);
			result.conjugate();
			return result;
		}

		template <typename _NumericT>
		Quaternion<_NumericT> Quaternion<_NumericT>::rotationTo (const Quaternion & other) const
		{
			return this->conjugatedQuaternion() * other;
		}

		template <typename _NumericT>
		Vector<4, _NumericT> & Quaternion<_NumericT>::vector ()
		{
			return m_vector;
		}

		template <typename _NumericT>
		const Vector<4, _NumericT> & Quaternion<_NumericT>::vector () const
		{
			return m_vector;
		}

		template class Quaternion<float>;
		template class Quaternion<double>;

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Quaternion)
		{
			testing("Construction");
			
			// Angle axis
			Quat q(R90, vec(1.0, 0.0, 0.0));
			Mat44 m = Mat44::rotatingMatrix(R90, vec(1.0, 0.0, 0.0));

			check(q.rotationAxis().equivalent(Vec3(1.0, 0.0, 0.0))) << "Rotation axis is correct";
			check(equivalent((RealT)R90, (RealT)q.rotationAngle())) << "Rotation angle is correct";

			testing("Multiplication");

			Vec3 si(15.14, -12.5, 4.55);

			Vec3 r1, r2;

			r1 = q * si;
			r2 = m * si;

			check(r1.equivalent(r2)) << "Represented rotation is same";
			
			Quat a(R90, Vec3(1, 0, 0).normalizedVector());
			Quat b(R90, Vec3(0, 1, 0).normalizedVector());
			Quat c = a.rotationTo(b);
			
			check((a * c).vector().equivalent(b.vector())) << "Rotations are equivalent";

			testing("Axis Extraction");

			Quat identity(IDENTITY);

			check(identity.extractAxis(X).equivalent(Vec3(1, 0, 0))) << "X axis is correct";
			check(identity.extractAxis(Y).equivalent(Vec3(0, 1, 0))) << "Y axis is correct";
			check(identity.extractAxis(Z).equivalent(Vec3(0, 0, 1))) << "Z axis is correct";

			check(a.extractAxis(X).equivalent(Vec3(1, 0, 0))) << "X axis is correct";
			check(a.extractAxis(Y).equivalent(Vec3(0, 0, 1))) << "Y axis is correct";
			check(a.extractAxis(Z).equivalent(Vec3(0, -1, 0))) << "Z axis is correct";
			
			testing("Rotation Matrix");
			
			Quat d(R360 * 0.34, Vec3(0.52, 0.1, -0.9).normalizedVector());
			Vec3 va(1, 0, 0), vb(0, 1, 0), vc(0, 0, 1);
			
			Mat44 t = d.rotatingMatrix();

			check(d.extractAxis(X).equivalent(t * va)) << "X axis is correct";
			check(d.extractAxis(Y).equivalent(t * vb)) << "Y axis is correct";
			check(d.extractAxis(Z).equivalent(t * vc)) << "Z axis is correct";
			
			// Transformation by matrix and transformation by multiplication is not correct
			// check((d * va).equivalent(t * va)) << "X rotation is correct";
			// check((d * vb).equivalent(t * vb)) << "Y rotation is correct";
			// check((d * vc).equivalent(t * vc)) << "Z rotation is correct";
		}
#endif
	}
}