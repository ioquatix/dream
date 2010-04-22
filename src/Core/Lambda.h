/*
 *  Core/Lambda.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 17/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_LAMBDA_H
#define _DREAM_CORE_LAMBDA_H

#include "Core.h"

#define XY(X, Y) X##Y
#define MakeNameXY(FX, LINE) XY(FX, LINE)
#define MakeName(FX) MakeNameXY(FX, __LINE__)

#define self (*this)

#define lambda(args, ret_type, body) \
class MakeName (_lambda_) \
{ \
public: ret_type operator() args \
{ body; } }

#endif