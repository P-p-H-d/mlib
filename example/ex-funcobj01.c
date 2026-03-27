#include <stdio.h>
#include "m-funcobj.h"
#include "m-array.h"

/* Simple POD type used by our shapes. It represents a 2D point. */
typedef struct {
    float x, y;
} Point;
/* Register is oplist globally */
#define M_OPL_Point() M_POD_OPLIST

/* Define a dynamic array of points (used by polygon) and register its oplist */
ARRAY_DEF(array_point, Point)
#define M_OPL_array_point_t() ARRAY_OPLIST(array_point, M_POD_OPLIST)

/*
 * Define a function-object interface named `shape`.
 * It exposes one callable operation with no argument and no return value.
 */
M_FUNC_OBJ_ITF_DEF(shape, void)

/* Concrete implementation #1 of 'shape': triangle. */
M_FUNC_OBJ_INS_DEF(triangle, shape, (), {
    /* Code of the called callable operation for a 'triangle' */
    printf("Draw Triangle: (%g,%g), (%g,%g), (%g,%g)\n",
           (double)self->a.x, (double)self->a.y,
           (double)self->b.x, (double)self->b.y,
           (double)self->c.x, (double)self->c.y);
}, 
    /* Internal fields of our object 'triangle' */
    /* Each line is name of the field, type of the field and the optional oplist of the type */
    (a, Point, M_POD_OPLIST),
    (b, Point, M_POD_OPLIST),
    (c, Point) /* We can also emit the OPLIST if we want since we have registered it */
)

/* Concrete implementation #2 of 'shape': polygon backed by a point array. */
M_FUNC_OBJ_INS_DEF(polygon, shape, (), {
    /* Code of the called callable operation for a 'polygon' */
    printf("Draw Polygon: ");
    // We iterate over all points of our polygon
    for M_EACH(point, self->points, array_point_t) {
        printf("(%g,%g) ", point->x, point->y);
    }
    printf("\n");
}, 
    /* Internal fields of our object 'polygon' */
    (points, array_point_t)
)

/* Store interface pointers so one array can hold different concrete types. */
ARRAY_DEF(Shapes_collection, shape_t, M_PTR_OPLIST)
/* Register it globally */
#define M_OPL_Shapes_collection_t() ARRAY_OPLIST(Shapes_collection, M_PTR_OPLIST)

int main(void)
{
    /* Create a heterogeneous collection of objects that all satisfy `shape`. */
    Shapes_collection_t shapes;
    Shapes_collection_init(shapes);

    /* Build one triangle and push it as a `shape` interface pointer. */
    triangle_t tri;
    triangle_init_with(tri, (Point){1, 0}, (Point){0, 1}, (Point){0, 0});
    Shapes_collection_push_back(shapes, triangle_as_interface(tri));

    /* Build a polygon from a dynamic list of points. */
    array_point_t points;
    array_point_init(points);
    array_point_push_back(points, (Point){0, 0});
    array_point_push_back(points, (Point){1, 0});
    array_point_push_back(points, (Point){1, 1});
    array_point_push_back(points, (Point){0, 1});
    polygon_t poly;
    polygon_init_with(poly, points);
    Shapes_collection_push_back(shapes, polygon_as_interface(poly));

    /* Runtime dispatch: both objects are called through the same interface. */
    for M_EACH(shape, shapes, Shapes_collection_t) {
        shape_call(*shape);
    }

    /* Clear container then concrete objects. */
    Shapes_collection_clear(shapes);
    triangle_clear(tri);
    polygon_clear(poly);
    array_point_clear(points);
    return 0;
}