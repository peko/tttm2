/* File: chanhull.h
 * Author: Pat Morin
 * Description: Implementation of Chan's output-sensitive convex 
 *              hull algorithm
 * Date: Sun Aug 19 16:15:58 EDT 2001
 */

#pragma once

#include "point.h"

/* Compute the convex hull of the point set s.  The hull is stored at 
 * location s+(return value) sorted in counterclockwise order
 */
int chanhull(point_t* s, int n);

