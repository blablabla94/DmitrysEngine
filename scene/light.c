/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
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

static void de_light_init(de_node_t* node)
{
	de_light_t* light = de_node_to_light(node);
	de_color_set(&light->color, 255, 255, 255, 255);
	light->radius = 2.0f;
	light->type = DE_LIGHT_TYPE_POINT;
	light->cone_angle = (float)M_PI;
	light->cone_angle_cos = -1.0f;
	light->cast_shadows = true;
}

void de_light_set_cast_shadows(de_light_t* light, bool value)
{
	DE_ASSERT(light);
	light->cast_shadows = value;
}

bool de_light_get_cast_shadows(const de_light_t* light)
{
	DE_ASSERT(light);
	return light->cast_shadows;
}

static void de_light_copy(de_node_t* src_node, de_node_t* dest_node)
{
	de_light_t* src = de_node_to_light(src_node);
	de_light_t* dest = de_node_to_light(dest_node);
	dest->color = src->color;
	dest->cone_angle = src->cone_angle;
	dest->cone_angle_cos = src->cone_angle_cos;
	dest->radius = src->radius;
	dest->type = src->type;
	dest->cast_shadows = src->cast_shadows;
}

static bool de_light_visit(de_object_visitor_t* visitor, de_node_t* node)
{
	de_light_t* light = de_node_to_light(node);
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_ENUM(visitor, "Type", &light->type);
	result &= de_object_visitor_visit_color(visitor, "Color", &light->color);
	result &= de_object_visitor_visit_float(visitor, "ConeAngle", &light->cone_angle);
	result &= de_object_visitor_visit_float(visitor, "CosConeAngle", &light->cone_angle_cos);
	result &= de_object_visitor_visit_float(visitor, "Radius", &light->radius);
	result &= de_object_visitor_visit_bool(visitor, "CastShadows", &light->cast_shadows);
	return result;
}

struct de_node_dispatch_table_t* de_light_get_dispatch_table(void)
{
	static de_node_dispatch_table_t tbl = {
		.init = de_light_init,
		.copy = de_light_copy,
		.visit = de_light_visit,
	};
	return &tbl;
}

void de_light_set_radius(de_light_t* light, float radius)
{
	DE_ASSERT(light);
	light->radius = de_maxf(FLT_EPSILON, radius);
}

float de_light_get_radius(const de_light_t* light)
{
	DE_ASSERT(light);
	return light->radius;
}

void de_light_set_cone_angle(de_light_t* light, float angle)
{
	DE_ASSERT(light);
	light->cone_angle = angle;
	light->cone_angle_cos = (float)cos(angle);
}

float de_light_get_cone_angle(const de_light_t* light)
{
	DE_ASSERT(light);
	return light->cone_angle;
}

void de_light_set_color(de_light_t* light, const de_color_t* color)
{
	DE_ASSERT(light);
	DE_ASSERT(color);
	light->color = *color;
}

void de_light_get_color(const de_light_t* light, de_color_t* color)
{
	DE_ASSERT(light);
	DE_ASSERT(color);
	*color = light->color;
}