/* File: throwaway.h
 * Author: Pat Morin
 * Description: Implementation of the throwaway heuristic
 * Date: Sun Aug 19 16:15:58 EDT 2001
 */

#pragma once

#include "primitives.h"

/* Preprocess the point set s by finding extreme points in 8
 * directions and eliminating points contained in the interior of the
 * convex hull of these 8 extreme points.  The eliminated points are
 * stored at the beginning of s.  The return value is the number of
 * points eliminated.  
 */
int throwaway_heuristic(point_t *s, int n);
