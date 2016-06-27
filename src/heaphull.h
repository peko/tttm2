/* File: heaphull.h
 * Author: Pat Morin
 * Description: Implementation of HeapHull2 algorithm
 * Date: Sun Aug 19 16:15:58 EDT 2001
 */

#pragma once

#include "primitives.h"

/* Compute the upper (dir = 1) or lower (dir = -1) hull of the point
 * set s.  The hull is stored in counterclockwise order beginning at
 * s+(return value). 
 */
int heap_upperlower_hull(point_t *s, int n, int dir);

/* Compute the convex hull of the point set s.  The hull is stored at 
 * location s+(return value) sorted in counterclockwise order
 */
int heaphull2(point_t *s, int n);

