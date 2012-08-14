//
//  Matrix.NEON.h
//  Dream
//
//  Created by Samuel Williams on 11/08/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_NUMERICS_MATRIX_NEON_H
#define _DREAM_NUMERICS_MATRIX_NEON_H

#include "Matrix.h"

#ifdef __arm__

#import <arm/arch.h>
#ifdef _ARM_ARCH_7

namespace Dream {
	namespace Numerics {
		// This is an optimised specialization for ARMv7 NEON:
		void multiply(Matrix<4, 4, float> & result, const Matrix<4, 4, float> & left, const Matrix<4, 4, float> & right);
		void multiply(Vector<4, float> & result, const Matrix<4, 4, float> & left, const Vector<4, float> & right);
	}
}

#endif

#endif

#endif
