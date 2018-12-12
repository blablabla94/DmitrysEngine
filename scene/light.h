/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

/**
* @brief Possible light types
*/
typedef enum de_light_type_e
{
	DE_LIGHT_TYPE_POINT,
	DE_LIGHT_TYPE_DIRECTIONAL,
	DE_LIGHT_TYPE_SPOT
} de_light_type_t;

/**
* @brief Common light component.
*
* Can be any possible light type (point, directional, spot)
*/
struct de_light_t
{
	de_node_t* parent_node;
	de_light_type_t type; /**< Actual type of light */
	float radius;         /**< Radius of point light */
	de_color_t color;     /**< Color of light */
};

/**
* @brief
*/
void de_light_init(de_light_t* light);

/**
* @brief
* @param ref
*/
void de_light_deinit(de_light_t* light);

void de_light_set_radius(de_node_t* node, float radius);