/*
 *  Numerics/Vector.impl.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 22/11/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_NUMERICS_VECTOR_H
#error This header should not be included manually. Include Vector.h instead.
#else

namespace Dream
{
	namespace Numerics
	{
#pragma mark -
#pragma mark Vector Traits

		template <typename NumericT>
		VectorConversionTraits<1, NumericT>::operator NumericT () const
		{
			const Vector<1, NumericT> &v = static_cast<const Vector<1, NumericT> &>(*this);

			return v[0];
		}
		
		template <unsigned E>
		bool VectorEqualityTraits<E, float>::equalWithinTolerance (const Vector<E, float> & other, const unsigned & ulps) const
		{
			const Vector<E, float> &v = static_cast<const Vector<E, float> &>(*this);

			for (unsigned int i = 0; i < E; ++i)
			{
				if (!Numerics::equalWithinTolerance(v[i], other[i], ulps))
					return false;
			}

			return true;
		}

		template <unsigned E>
		bool VectorEqualityTraits<E, double>::equalWithinTolerance (const Vector<E, double> & other, const unsigned & ulps) const
		{
			const Vector<E, double> &v = static_cast<const Vector<E, double> &>(*this);

			for (unsigned int i = 0; i < E; ++i)
			{
				if (!Numerics::equalWithinTolerance(v[i], other[i], ulps))
					return false;
			}

			return true;
		}

		template <unsigned E>
		bool VectorEqualityTraits<E, float>::equivalent (const Vector<E, float> & other) const
		{
			const Vector<E, float> &v = static_cast<const Vector<E, float> &>(*this);

			for (unsigned int i = 0; i < E; ++i)
			{
				if (!Numerics::equivalent(v[i], other[i]))
					return false;
			}

			return true;
		}

		template <unsigned E>
		bool VectorEqualityTraits<E, double>::equivalent (const Vector<E, double> & other) const
		{
			const Vector<E, double> &v = static_cast<const Vector<E, double> &>(*this);

			for (unsigned int i = 0; i < E; ++i)
			{
				if (!Numerics::equivalent(v[i], other[i]))
					return false;
			}

			return true;
		}

		template <typename NumericT>
		Vector<3, NumericT> surfaceNormal (const Vector<3, NumericT> & a, const Vector<3, NumericT> & b, const Vector<3, NumericT> & c)
		{
			Vector<3, NumericT> a1 = b - a;
			Vector<3, NumericT> b1 = c - b;

			return a1.cross(b1).normalize();
		}
		
		template <typename NumericT>
		Vector<2, NumericT> VectorTraits<2, NumericT>::normal () const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);
			Vector<3, NumericT> up(0.0, 0.0, 1.0);
		
			return (v << 0.0).cross(up).reduce();
		}
		
		template <typename NumericT>
		NumericT VectorTraits<2, NumericT>::aspectRatio () const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);
			
			return v[WIDTH] / v[HEIGHT];
		}
		
		template <typename NumericT>
		Vector<2, NumericT> VectorTraits<2, NumericT>::shrinkToFitAspectRatio(NumericT n) const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);

			Vector<2, NumericT> result;
			
			result = v;
			result[WIDTH] = n * result[HEIGHT];
			
			if (result[WIDTH] <= v[WIDTH]) {
				return result;
			}
			
			result = v;
			result[HEIGHT] = (1.0 / n) * result[WIDTH];
			
			return result;
		}
		
		template <typename NumericT>
		Vector<2, NumericT> VectorTraits<2, NumericT>::shrinkToFitAspectRatio(const Vector<2, NumericT> & other) const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);
			
			return this->shrinkToFitAspectRatio(other.aspectRatio());
		}
		
		template <typename NumericT>
		Vector<2, NumericT> VectorTraits<2, NumericT>::expandToFitAspectRatio(NumericT n) const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);

			Vector<2, NumericT> result;
			
			result = v;
			result[WIDTH] = n * result[HEIGHT];
			
			if (result[WIDTH] >= v[WIDTH]) {
				return result;
			}
			
			result = v;
			result[HEIGHT] = (1.0 / n) * result[WIDTH];
			
			return result;
		}
		
		template <typename NumericT>
		Vector<2, NumericT> VectorTraits<2, NumericT>::expandToFitAspectRatio(const Vector<2, NumericT> & other) const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);
			
			return this->expandToFitAspectRatio(other.aspectRatio());
		}

		template <typename NumericT>
		Vector<3, NumericT> VectorTraits<3, NumericT>::cross (const Vector<3, NumericT> & other) const
		{
			const VectorT &v = *static_cast<const VectorT*>(this);
			Vector<3, NumericT> result;

			result[0] = v[1] * other[2] - v[2] * other[1];
			result[1] = v[2] * other[0] - v[0] * other[2];
			result[2] = v[0] * other[1] - v[1] * other[0];

			return result;
		}

		template <typename NumericT>
		Vector<3, NumericT> VectorTraits<3, NumericT>::normal (const Vector<3, NumericT> & other) const
		{
			return cross(other).normalize();
		}

		template <typename NumericT>
		Vector<3, NumericT> VectorTraits<3, NumericT>::normal (const Vector<3, NumericT> & a, const Vector<3, NumericT> & b) const
		{
			return surfaceNormal (*static_cast<const VectorT*>(this), a, b);
		}

		template <typename NumericT>
		Vector<4, NumericT> crossProduct (const Vector<4, NumericT> & u, const Vector<4, NumericT> & v, const Vector<4, NumericT> & w)
		{
			Vector<4, NumericT> result;

			// intermediate values
			NumericT a, b, c, d, e, f;

			// calculate intermediate values.
			a = (v[0] * w[1]) - (v[1] * w[0]);
			b = (v[0] * w[2]) - (v[2] * w[0]);
			c = (v[0] * w[3]) - (v[3] * w[0]);
			d = (v[1] * w[2]) - (v[2] * w[1]);
			e = (v[1] * w[3]) - (v[3] * w[1]);
			f = (v[2] * w[3]) - (v[3] * w[2]);

			// calculate the result-vector components.
			result[0] =   (u[1] * f) - (u[2] * e) + (u[3] * d);
			result[1] = - (u[0] * f) + (u[2] * c) - (u[3] * b);
			result[2] =   (u[0] * e) - (u[1] * c) + (u[3] * a);
			result[3] = - (u[0] * d) + (u[1] * b) - (u[2] * a);

			return result;
		}

		template <typename NumericT>
		Vector<4, NumericT> VectorTraits<4, NumericT>::cross (const Vector<4, NumericT> & v, const Vector<4, NumericT> & w)
		{
			return crossProduct(*this, v, w);
		}

#pragma mark -
#pragma mark Vector

		template <unsigned E, typename NumericT>
		Vector<E, NumericT>::Vector (const NumericT & x)
		{
			for (unsigned i = 0; i < E; i += 1) {
				m_vector[i] = x;
			}
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT>::Vector (const NumericT & x, const NumericT & y)
		{
			BOOST_STATIC_ASSERT(E == 2);
		
			m_vector[0] = x;
			m_vector[1] = y;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT>::Vector (const NumericT & x, const NumericT & y, const NumericT & z)
		{
			BOOST_STATIC_ASSERT(E == 3);
		
			m_vector[0] = x;
			m_vector[1] = y;
			m_vector[2] = z;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT>::Vector (const NumericT & x, const NumericT & y, const NumericT & z, const NumericT & w)
		{
			BOOST_STATIC_ASSERT(E == 4);
			
			m_vector[0] = x;
			m_vector[1] = y;
			m_vector[2] = z;
			m_vector[3] = w;
		}

		template <unsigned E, typename NumericT> template <typename otherNumericT>
		Vector<E, NumericT>::Vector (const Vector<E, otherNumericT> & other)
		{
			(*this) = other;
		}

		template <unsigned E, typename NumericT>
		void Vector<E, NumericT>::zero ()
		{
			memset(m_vector, 0, sizeof(m_vector));
		}

		template <unsigned E, typename NumericT>
		void Vector<E, NumericT>::loadIdentity (const NumericT & value)
		{
			(*this) = value;
		}

		template <unsigned E, typename NumericT>
		bool Vector<E, NumericT>::isZero () const
		{
			return !this->sum();
		}

		template <unsigned E, typename NumericT> template <unsigned E2, typename otherNumericT>
		void Vector<E, NumericT>::set (const Vector<E2, otherNumericT> & other)
		{
			using namespace boost::mpl;
			typedef typename min<int_<E>, int_<E2> >::type S;

			for (unsigned i = 0; i < S::value; ++i)
				this->m_vector[i] = (NumericT)other[i];
		}

		template <unsigned E, typename NumericT>
		void Vector<E, NumericT>::set (const NumericT * other)
		{
			memcpy(this->m_vector, other, sizeof(m_vector));
		}

		template <unsigned E, typename NumericT>
		void Vector<E, NumericT>::set (const NumericT * other, unsigned count, unsigned offset)
		{
			memcpy(this->m_vector + offset, other, sizeof(NumericT) * count);
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::operator== (const OtherT & other) const
		{
			return ! (*this != other);
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::operator!= (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] != other[i]) return true;

			return false;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::lessThan (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] >= other[i])
					return false;
			return true;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::greaterThan (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] <= other[i])
					return false;
			return true;
		}

		// Numeric comparisons
		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::lessThanOrEqual (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] > other[i])
					return false;
			return true;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::greaterThanOrEqual (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] < other[i])
					return false;
			return true;
		}

		// Set comparisons
		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::operator< (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] > other[i]) return false;
				else if (m_vector[i] < other[i]) return true;

			return false;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::operator> (const OtherT & other) const
		{
			for (unsigned i = 0; i < E; ++i)
				if (m_vector[i] < other[i]) return false;
				else if (m_vector[i] > other[i]) return true;

			return false;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::operator<= (const OtherT & other) const
		{
			if (operator!=(other))
				return operator<(other);

			return true;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		bool Vector<E, NumericT>::operator>= (const OtherT & other) const
		{
			if (operator!=(other))
				return operator>(other);

			return true;
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		NumericT Vector<E, NumericT>::dot (const OtherT & other) const
		{
			NumericT result = 0;

			for (unsigned i = 0; i < E; ++i)
			{
				result += m_vector[i] * other[i];
			}

			return result;
		}
		
		template <unsigned E, typename NumericT> template <typename OtherT>
		NumericT Vector<E, NumericT>::angleBetween (const OtherT & other) const
		{			
			NumericT r = this->dot(other) / (this->length() * other.length());
			
			return Number<NumericT>::acos(Number<NumericT>::clamp(r, (NumericT)-1.0, (NumericT)1.0));
		}		

		template <unsigned E, typename NumericT>
		NumericT Vector<E, NumericT>::sum () const
		{
			NumericT result = 0;

			for (unsigned i = 0; i < E; ++i)
				result += m_vector[i];

			return result;
		}

		template <unsigned E, typename NumericT>
		NumericT Vector<E, NumericT>::product () const
		{
			NumericT result = 1;
			for (unsigned i = 0; i < E; ++i)
				result *= m_vector[i];

			return result;
		}

		// Given a size vector (x, y, [z]), distribute i as if
		// it was an index into that space
		template <unsigned E, typename NumericT>
		Vector<E, NumericT> Vector<E, NumericT>::distribute (NumericT k) const
		{
			Vector r(ZERO);
			NumericT m = this->product();

			ensure(m);

			for (unsigned i = E; i > 0; i -= 1)
			{
				m /= m_vector[i-1];
				r[i-1] = Number<NumericT>::floor(k / m);
				k = Number<NumericT>::mod(k, m);
			}

			ensure(m == 1);

			return r;
		}

		// Given a size vector (this) and a coordinate, return an index
		// for access into a 1E array.
		template <unsigned E, typename NumericT>
		NumericT Vector<E, NumericT>::index (const Vector<E, NumericT> & coord) const
		{
			NumericT idx = 0;
			NumericT m = 1;

			//		x(1) + y(sx) + z(sx*sy)
			for (unsigned i = 0; i < E; i += 1)
			{
				idx += coord[i] * m;
				m *= m_vector[i];
			}

			return idx;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::clamp (const unsigned i, const NumericT & min, const NumericT & max)
		{
			m_vector[i] = Math::clamp(m_vector[i], min, max);

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::clamp (const NumericT & min, const NumericT & max)
		{
			for (unsigned int i = 0; i < E; ++i)
				m_vector[i] = Number<NumericT>::clamp(m_vector[i], min, max);

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::abs ()
		{
			for (unsigned int i = 0; i < E; ++i)
			{
				m_vector[i] = Math::abs(m_vector[i]);
			}

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::floor ()
		{
			for (unsigned i = 0; i < E; ++i)
				m_vector[i] = Number<NumericT>::floor(m_vector[i]);

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::ceil ()
		{
			for (unsigned i = 0; i < E; ++i)
				m_vector[i] = Number<NumericT>::ceil(m_vector[i]);

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::frac ()
		{
			for (unsigned i = 0; i < E; ++i)
				m_vector[i] = m_vector[i] - Number<NumericT>::floor(m_vector[i]);

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::normalize (const NumericT & toLength)
		{
			NumericT n = this->length();

			if (n == 0)
			{
				return *this;
			}

			//ensure(n > 0 && "Trying to normalize zero-length vector!");

			if (toLength != n)
			{
				NumericT factor = toLength / n;
				*this *= factor;
			}

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> Vector<E, NumericT>::normalizedVector (const NumericT & toLength) const
		{
			// Copy the vector
			Vector<E, NumericT> copy(*this);

			// Normalize the copy and return it.
			return copy.normalize(toLength);
		}

		template <unsigned E, typename NumericT> template <typename OtherT>
		void Vector<E, NumericT>::constrain (const Vector<E, NumericT> & with, const OtherT & limits)
		{
			for (unsigned int i = 0; i < E; ++i)
			{
				if (limits[i] < 0)
				{
					m_vector[i] = (m_vector[i] < with[i] ? m_vector[i] : with[i]) * -(NumericT)limits[i];
				} else if (limits[i] > 0)
				{
					m_vector[i] = (m_vector[i] > with[i] ? m_vector[i] : with[i]) * (NumericT)limits[i];
				}
			}
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::negate ()
		{
			for (unsigned i = 0; i < E; ++i)
				m_vector[i] = -m_vector[i];

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> Vector<E, NumericT>::operator- () const
		{
			return this->copy().negate();
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> & Vector<E, NumericT>::inverse ()
		{
			for (unsigned i = 0; i < E; ++i)
				m_vector[i] = !m_vector[i];

			return *this;
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> Vector<E, NumericT>::operator! () const
		{
			return this->copy().inverse();
		}

		template <unsigned E, typename NumericT>
		Vector<E, NumericT> Vector<E, NumericT>::lerp (const NumericT & t, Vector<E, NumericT> other) const
		{
			return (*this) + t * (other - *this);
		}

		// Is there a more generic way to do this?
		template <unsigned E, typename NumericT>
		bool Vector<E, NumericT>::increment (const Vector<E, NumericT> & max)
		{
			for (unsigned i = 0; i < E; ++i)
			{
				if ((m_vector[i] + 1) < max[i])
				{
					m_vector[i] += 1;

					if (i) m_vector[i-1] = 0;
					return true;
				}
			}

			// Could not increment
			return false;
		}

		// Pack the vector into a single variable
		template <unsigned E, typename NumericT> template <typename PackedT>
		void Vector<E, NumericT>::pack (unsigned bits, PackedT & p) const
		{
			unsigned mask = ((1 << bits) - 1);
			for (unsigned i = 0; i < E; ++i)
			{
				p <<= bits;

				// Mask out higher order bits
				p += m_vector[i] & mask;
			}
		}

		// Unpack elements of a vector from a single variable
		template <unsigned E, typename NumericT> template <typename PackedT>
		void Vector<E, NumericT>::unpack (unsigned bits, const PackedT & p)
		{
			for (unsigned i = 0; i < E; ++i)
			{
				m_vector[E-(i+1)] = (p >> (bits*i)) & ((1 << bits) - 1);
			}
		}
	}
}

#endif