//
//  Numerics/Average.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/10/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_NUMERICS_AVERAGE_H
#define _DREAM_NUMERICS_AVERAGE_H

#include "Numerics.h"

namespace Dream
{
	namespace Numerics
	{
		/** This class can assist in the calculation of mathematical averages.
		 */
		template <typename type_t>
		class Average
		{
		protected:
			/// The number of samples taken so far.
			uint32_t m_times;
			/// The current total.
			/// @todo This value might overflow. Is there a way to fix this behaviour?
			type_t m_value;

			/// Add a set of samples to the average.
			void addSamples (const type_t & v, const uint32_t & t)
			{
				m_times += t;
				m_value += v;
			}

		public:
			/// Default constructor
			Average () : m_times (0), m_value (0)
			{
			}

			/// Add a single sample to the average.
			void addSample (const type_t & v)
			{
				m_times += 1;
				m_value += v;
			}

			/// Add samples from another instance of Average.
			void addSamples (const Average<type_t> & other)
			{
				addSamples(other.m_value, other.m_times);
			}

			/// Calculate the average value.
			/// @returns The average value.
			type_t average ()
			{
				if (m_times == 0) return 0;

				return m_value / (type_t)m_times;
			}

			/// Check if any samples have been added.
			/// @returns true if there are samples.
			bool hasSamples ()
			{
				return m_times != 0;
			}

			/// The number of samples taken.
			uint32_t numberOfSamples () const
			{
				return m_times;
			}

			/// Helper for adding a sample. Same as addSample.
			void operator+= (const type_t & sample)
			{
				addSample(sample);
			}

			/// Helper for adding a samples. Same as addSamples.
			void operator+= (const Average<type_t> & other)
			{
				addSamples(other);
			}
		};

	}
}

#endif
