//!
//! This file defines the trackball class
//!
#pragma once
#ifndef _TRACKBALL_H_
#define _TRACKBALL_H_

#include "common.h"

//!
//! following the implementation of http://image.diku.dk/research/trackballs/index.html
//!
class Trackball {
private:
    float     radius = 1.0f;
    GLboolean inverse_mode = false;
    cy::Matrix4f matrix = cy::Matrix4f::MatrixIdentity();
    cy::Matrix4f matrix_prev = cy::Matrix4f::MatrixIdentity();
    cy::Point3f  position;
    cy::Point3f  position_prev;
    float position_surf[2];

public:
    Trackball() {}
    Trackball(bool i) : inverse_mode(i) {}

    void SetRadius(const float r) { radius = r; }
    void SetInverseMode(bool r) { inverse_mode = r; }

    /**
     * @brief BeginDrag/Zoom: initialize drag/zoom
     * @param x: previous x position
     * @param y: previous y position
     */
    void BeginDrag(float x, float y)
    {
	position_prev = proj2surf(x, y);
	position_surf[0] = x;
	position_surf[1] = y;
	matrix_prev = matrix;
    }

    /**
     * @brief Drag/Zoom: execute Drag/Zoom
     * @param x: current x position
     * @param y: current y position
     */
    void Drag(float x, float y)
    {
	cy::Matrix4f rot;
	// get direction
	position = proj2surf(x, y);
	cy::Point3f dir = (position_prev.Cross(position)).GetNormalized();
	dir = inverse_mode ? -dir : dir;
	// compute rotation angle
	float angle = position_prev.Dot(position) / position_prev.Length() / position.Length();
	if (angle > 0.99999999999) { // to prevent position_prev == position, this will cause invalid value
	    return;
	}
	else { // compute rotation
	    rot.SetRotation(dir, acos(angle));
	}
	matrix = rot * matrix_prev;
    }

    /**
     * @brief matrix: trackball matrix accessor
     * @return current trackball matrix
     */
    cy::Matrix4f Matrix()
    {
	return matrix;
    }

    void Reset() { matrix = cy::Matrix4f::MatrixIdentity(); }

private:
    /**
     * @brief proj2surf: project (x,y) mouse pos on surface
     * @param x: X position
     * @param y: Y position
     * @return projected position
     */
    cy::Point3f proj2surf(const float x, const float y) const
    {
	float r = x * x + y * y;
	float R = radius * radius;
	float z = r > R / 2 ? R / 2 / sqrt(r) : sqrt(R - r);
	return cy::Point3f(x, y, z);
    }

};

#endif//_TRACKBALL_H_
