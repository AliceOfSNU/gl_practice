#include "class.h"

object* obj_init()
{
	object* newobj = new object();
	newobj->v_list = new std::vector<vertex*>();
	newobj->e_list = new std::vector<edge*>();
	newobj->f_list = new std::vector<face*>();
	newobj->vertices = new std::vector<GLfloat>();
	newobj->vertexIndices = new std::vector<unsigned int>();
	return newobj;
}

vertex* vtx_init()
{
	vertex* newvtx = new vertex();
	newvtx->e_list = new std::vector<edge*>();
	newvtx->f_list = new std::vector<face*>();
	newvtx->v_new = NULL;
	newvtx->idx = -1;
	return newvtx;
}

edge* edge_init()
{
	edge* newedge = new edge();
	newedge->f_list = new std::vector<face*>();
	newedge->v1 = NULL;
	newedge->v2 = NULL;
	newedge->edge_pt = NULL;
	return newedge;
}

face* face_init()
{
	face* newface = new face();
	newface->v_list = new std::vector<vertex*>();
	newface->e_list = new std::vector<edge*>();
	newface->face_pt = NULL;
	return newface;
}

vertex* add_vertex(object* obj, const coord& coord)
{
	vertex* newvtx = vtx_init();
	newvtx->xyz.x = coord.x;
	newvtx->xyz.y = coord.y;
	newvtx->xyz.z = coord.z;
	newvtx->idx = obj->v_list->size();
	obj->v_list->push_back(newvtx);
	return newvtx;
}

vertex* add_vertex(object* obj, vertex*  vtx) {
	if (std::find(obj->v_list->begin(), obj->v_list->end(), vtx) == obj->v_list->end()) {
		// not already in the list
	}
	vtx->idx = obj->v_list->size();
	obj->v_list->push_back(vtx);
	return vtx;
}

edge* find_edge(object* obj, vertex* v1, vertex* v2)
{
	std::vector<edge*>* v1_edgeList = v1->e_list;
	for(int i = 0; i < v1_edgeList->size(); i++)
	{
		if((*v1_edgeList)[i]->v1 == v2 || (*v1_edgeList)[i]->v2 == v2)
		{
			return (*v1_edgeList)[i];
		}
	}
	return NULL;
}

edge* add_edge(object* obj, vertex* v1, vertex* v2)
{
	edge* newedge = edge_init();
	newedge->v1 = v1;
	newedge->v2 = v2;
	v1->e_list->push_back(newedge);
	v2->e_list->push_back(newedge);
	obj->e_list->push_back(newedge);
	return newedge;
}

face* add_face(object* obj, const std::vector<int>& vertexIndices)
{
	face* newface = face_init();
	int n = vertexIndices.size();
	for (int i = 0; i < n; i++)
	{
		vertex* v1 = (*(obj->v_list))[vertexIndices[i]];
		vertex* v2 = (*(obj->v_list))[vertexIndices[(i+1)%n]];
		v1->f_list->push_back(newface);

		edge* temp = find_edge(obj, v1, v2);
		if(!temp) temp = add_edge(obj, v1, v2);

		temp->f_list->push_back(newface);
		newface->e_list->push_back(temp);
		newface->v_list->push_back(v1);
	}
	obj->f_list->push_back(newface);
	return newface;
}

coord add(const coord& ord1, const coord& ord2)
{
	coord temp;
	temp.x = ord1.x + ord2.x;
	temp.y = ord1.y + ord2.y;
	temp.z = ord1.z + ord2.z;
	return temp;
}

coord sub(const coord& ord1, const coord& ord2)
{
	coord temp;
	temp.x = ord1.x - ord2.x;
	temp.y = ord1.y - ord2.y;
	temp.z = ord1.z - ord2.z;
	return temp;
}

coord mul(const coord& ord1, GLfloat m)
{
	coord temp;
	temp.x = ord1.x * m;
	temp.y = ord1.y * m;
	temp.z = ord1.z * m;
	return temp;
}

coord div(const coord& ord1, GLfloat d)
{
	coord temp;
	temp.x = ord1.x / d;
	temp.y = ord1.y / d;
	temp.z = ord1.z / d;
	return temp;
}

coord cross(const coord& ord1, const coord& ord2)
{
	coord temp;
	temp.x = ord1.y * ord2.z - ord1.z * ord2.y;
	temp.y = ord1.z * ord2.x - ord1.x * ord2.z;
	temp.z = ord1.x * ord2.y - ord1.y * ord2.x;
	return temp;
}

void setNorm(object* obj)
{
	for (int i = 0; i < obj->f_list->size(); i++)
	{
		face* temp = (*(obj->f_list))[i];
		coord v01 = sub((*(temp->v_list))[1]->xyz, (*(temp->v_list))[0]->xyz);
		coord v12 = sub((*(temp->v_list))[2]->xyz, (*(temp->v_list))[1]->xyz);
		coord crs = cross(v01, v12);
		crs.normalize();
		temp->norm = crs;
	}

	for (int i = 0; i < obj->v_list->size(); i++)
	{
		coord sum;
		std::vector<face*>* temp = (*(obj->v_list))[i]->f_list;
		int n = temp->size();
		for (int j = 0; j < n; j++)
		{
			sum.add((*temp)[j]->norm);
		}
		sum.div((GLfloat)n);
		sum.normalize();
		(*(obj->v_list))[i]->avg_norm = sum;
	}
}

void aggregate_vertices(object* obj)
{
	obj->vertices->clear();
	obj->vertexIndices->clear();

	for (int i = 0; i < obj->v_list->size(); i++)
	{
		coord temp_pos = (*(obj->v_list))[i]->xyz;
		coord temp_norm = (*(obj->v_list))[i]->avg_norm;
		obj->vertices->push_back(temp_pos.x);
		obj->vertices->push_back(temp_pos.y);
		obj->vertices->push_back(temp_pos.z);
		obj->vertices->push_back(temp_norm.x);
		obj->vertices->push_back(temp_norm.y);
		obj->vertices->push_back(temp_norm.z);
	}

	if (obj->vertices_per_face == 3)
	{
		for (int i = 0; i < obj->f_list->size(); i++)
		{
			std::vector<vertex*>* temp = (*(obj->f_list))[i]->v_list;
			obj->vertexIndices->push_back((*temp)[0]->idx);
			obj->vertexIndices->push_back((*temp)[1]->idx);
			obj->vertexIndices->push_back((*temp)[2]->idx);
		}
	}

	else if (obj->vertices_per_face == 4)
	{
		for (int i = 0; i < obj->f_list->size(); i++)
		{
			std::vector<vertex*>* temp = (*(obj->f_list))[i]->v_list;
			obj->vertexIndices->push_back((*temp)[0]->idx);
			obj->vertexIndices->push_back((*temp)[1]->idx);
			obj->vertexIndices->push_back((*temp)[2]->idx);
			obj->vertexIndices->push_back((*temp)[2]->idx);
			obj->vertexIndices->push_back((*temp)[3]->idx);
			obj->vertexIndices->push_back((*temp)[0]->idx);
		}
	}
}

object* cube()
{
	object* newobj = obj_init();
	newobj->vertices_per_face = 4;
	for (int x = -1; x <= 1; x += 2)
	{
		for (int y = -1; y <= 1; y += 2)
		{
			for (int z = -1; z <= 1; z += 2)
			{
				add_vertex(newobj, coord((GLfloat)x, (GLfloat)y, (GLfloat)z));
			}
		}
	}
	add_face(newobj, { 0,2,6,4 });
	add_face(newobj, { 0,4,5,1 });
	add_face(newobj, { 0,1,3,2 });
	add_face(newobj, { 2,3,7,6 });
	add_face(newobj, { 6,7,5,4 });
	add_face(newobj, { 1,5,7,3 });

	setNorm(newobj);

	aggregate_vertices(newobj);

	return newobj;
}


object* donut()
{
	object* m = obj_init();
	m->vertices_per_face = 4;
	int i;
	coord v[] = {
		{ -2, -.5, -2 }, { -2, -.5,  2 }, {  2, -.5, -2 }, {  2, -.5,  2 },
		{ -1, -.5, -1 }, { -1, -.5,  1 }, {  1, -.5, -1 }, {  1, -.5,  1 },
		{ -2,  .5, -2 }, { -2,  .5,  2 }, {  2,  .5, -2 }, {  2,  .5,  2 },
		{ -1,  .5, -1 }, { -1,  .5,  1 }, {  1,  .5, -1 }, {  1,  .5,  1 },
	};

	for (i = 0; i < 16; i++) add_vertex(m, coord(v[i].x, v[i].y, v[i].z));
	add_face(m, { 4, 5, 1, 0 });
	add_face(m, { 3, 1, 5, 7 });
	add_face(m, { 0, 2, 6, 4 });
	add_face(m, { 2, 3, 7, 6 });

	add_face(m, { 8, 9, 13, 12 });
	add_face(m, { 15, 13, 9, 11 });
	add_face(m, { 12, 14, 10, 8 });
	add_face(m, { 14, 15, 11, 10 });

	add_face(m, { 0, 1, 9, 8 });
	add_face(m, { 1, 3, 11, 9 });
	add_face(m, { 2, 0, 8, 10 });
	add_face(m, { 3, 2, 10, 11 });

	add_face(m, { 12, 13, 5, 4 });
	add_face(m, { 13, 15, 7, 5 });
	add_face(m, { 14, 12, 4, 6 });
	add_face(m, { 15, 14, 6, 7 });

	setNorm(m);

	aggregate_vertices(m);

	return m;
}

object* star()
{
	object* m = obj_init();
	m->vertices_per_face = 3;
	int ang, i;
	double rad;
	coord v[15];

	for (i = 0; i < 5; i++) {
		ang = i * 72;
		rad = ang * 3.1415926 / 180;
		v[i].x = 2.2 * cos(rad); v[i].y = 2.2 * sin(rad); v[i].z = 0;

		rad = (ang + 36) * 3.1415926 / 180;
		v[i + 5].x = v[i + 10].x = cos(rad);
		v[i + 5].y = v[i + 10].y = sin(rad);
		v[i + 5].z = .5;
		v[i + 10].z = -.5;
	}

	for (i = 0; i < 15; i++) add_vertex(m, coord(v[i].x, v[i].y, v[i].z));
	add_face(m, { 0, 5, 9 });
	add_face(m, { 1, 6, 5 });
	add_face(m, { 2, 7, 6 });
	add_face(m, { 3, 8, 7 });
	add_face(m, { 4, 9, 8 });

	add_face(m, { 0, 14, 10 });
	add_face(m, { 1, 10, 11 });
	add_face(m, { 2, 11, 12 });
	add_face(m, { 3, 12, 13 });
	add_face(m, { 4, 13, 14 });

	add_face(m, { 0, 10, 5 });
	add_face(m, { 1, 5, 10 });
	add_face(m, { 1, 11, 6 });
	add_face(m, { 2, 6, 11 });
	add_face(m, { 2, 12, 7 });
	add_face(m, { 3, 7, 12 });
	add_face(m, { 3, 13, 8 });
	add_face(m, { 4, 8, 13 });
	add_face(m, { 4, 14, 9 });
	add_face(m, { 0, 9, 14 });

	setNorm(m);

	aggregate_vertices(m);

	return m;
}

bool is_holeEdge(edge* e)
{
	return e->f_list->size() == 1;
}

bool is_holeVertex(vertex* v)
{
	return v->e_list->size() != v->f_list->size();
}

vertex* face_point(face* f)
{
	if (f->face_pt) return f->face_pt;
	f->face_pt = vtx_init();

	// iterate over the vertices and average the coords.
	int size = f->v_list->size();
	if (size < 1) return f->face_pt;

	for (int i = 0; i < size; i++)
	{
		f->face_pt->xyz.add((*(f->v_list))[i]->xyz);
	}
	
	f->face_pt->xyz.div(size);
	return f->face_pt;
	// not assigned f_list nor e_list nor index.
}

vertex* edge_point(edge* e)
{
	if (e->edge_pt) return e->edge_pt;
	e->edge_pt = vtx_init();
	if (is_holeEdge(e)) {
		// midpoint
		e->edge_pt->xyz = div(add(e->v1->xyz, e->v2->xyz),2.f);
	}
	else {
		// average of all face points and end points.
		for (int i = 0; i < e->f_list->size() ; i++)
		{
			e->edge_pt->xyz.add(face_point((*(e->f_list))[i])->xyz);
		}
		e->edge_pt->xyz.add(e->v1->xyz);
		e->edge_pt->xyz.add(e->v2->xyz);
		e->edge_pt->xyz.div(e->f_list->size() + 2.f);
	}
	return e->edge_pt; // delete this line after you fill in the blank.
}

vertex* vertex_point(vertex* v)
{
	if (v->v_new) return v->v_new;
	v->v_new = vtx_init();
	
	if (is_holeVertex(v)) {
		v->v_new->xyz = v->xyz;
		int count = 1;
		// iterate over the holeEdges attached to the vertex
		for (int i = 0; i < v->e_list->size(); ++i) {
			edge* edge_temp = (*(v->e_list))[i];
			if (is_holeEdge(edge_temp)) {
				v->v_new->xyz.add(div(add(edge_temp->v1->xyz, edge_temp->v2->xyz),2.f));
				count++;
			}
		}
		v->v_new->xyz.div(count);

	}
	else {
		// f_list -> size == e_list_size in non-hole vtx.
		int n = v->f_list->size();
		// face points
		for (int i = 0; i < n; ++i) {
			v->v_new->xyz.add(div((face_point((*(v->f_list))[i])->xyz),n));
		}
		// edges
		for (int i = 0; i < v->e_list->size(); ++i) {
			v->v_new->xyz.add(div((add((*(v->e_list))[i]->v1->xyz, (*(v->e_list))[i]->v2->xyz)), n));
		}
		// this vertex
		v->v_new->xyz.add(mul(v->xyz, n - 3));
		// divide by n (number of faces)
		v->v_new->xyz.div(n);
	}
	return v->v_new;
}

object* catmull_clark(object* obj)
{
	object* newobj = obj_init();
	newobj->vertices_per_face = 4;


	for (int i = 0; i < obj->f_list->size(); ++i) {
		// for each face
		face* f = (*(obj->f_list))[i];
		std::vector<int> indx = std::vector<int>();
		for (int j = 0; j < obj->vertices_per_face; j++) {
			// for each vertex in the tri/quad
			indx.clear();
			// my vertex point
			vertex* v = vertex_point((*(f->v_list))[j]);
			indx.push_back(add_vertex(newobj, v)->idx);
			// right edge point
			v = edge_point(find_edge(obj, (*(f->v_list))[j], (*(f->v_list))[(j + 1) % obj->vertices_per_face]));
			indx.push_back(add_vertex(newobj, v)->idx);
			// face point
			v = face_point(f);
			indx.push_back(add_vertex(newobj, v)->idx);
			// left edge point
			v = edge_point(find_edge(obj, (*(f->v_list))[j], (*(f->v_list))[(j + obj->vertices_per_face - 1) % obj->vertices_per_face]));
			indx.push_back(add_vertex(newobj, v)->idx);

			// add_face will also add the edges and configure vertices.
			add_face(newobj, indx);
		}
	}
	
	setNorm(newobj);

	aggregate_vertices(newobj);

	std::cout << "completed subdivide surface , vertices " << newobj->v_list->size() << std::endl;
	delete obj;

	return newobj;
}
