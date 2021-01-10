#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "scene_parser.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "object3d.hpp"
#include "group.hpp"
#include "mesh.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "transform.hpp"

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

SceneParser::SceneParser(const char *filename)
{

    // initialize some reasonable default values
    group = nullptr;
    camera = nullptr;
    background_color = Vector3f(0.5, 0.5, 0.5);
    num_lights = 0;
    lights = nullptr;
    num_materials = 0;
    materials = nullptr;
    current_material = nullptr;

    // parse the file
    assert(filename != nullptr);
    const char *ext = &filename[strlen(filename) - 4];

    if (strcmp(ext, ".txt") != 0)
    {
        printf("wrong file name extension\n");
        exit(0);
    }
    file = fopen(filename, "r");

    if (file == nullptr)
    {
        printf("cannot open scene file\n");
        exit(0);
    }
    parseFile();
    fclose(file);
    file = nullptr;
}

SceneParser::~SceneParser()
{

    delete group;
    delete camera;

    int i;
    for (i = 0; i < num_materials; i++)
    {
        delete materials[i];
    }
    delete[] materials;
}

// ====================================================================
// ====================================================================

void SceneParser::parseFile()
{
    //
    // at the top level, the scene can have a camera,
    // background color and a group of objects
    // (we add lights and other things in future assignments)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    while (getToken(token))
    {
        if (!strcmp(token, "PerspectiveCamera"))
        {
            parsePerspectiveCamera();
        }
        else if (!strcmp(token, "Background"))
        {
            parseBackground();
        }
        else if (!strcmp(token, "Materials"))
        {
            parseMaterials();
        }
        else if (!strcmp(token, "Group"))
        {
            group = parseGroup();
        }
        else
        {
            printf("Unknown token in parseFile: '%s'\n", token);
            exit(0);
        }
    }
}

// ====================================================================
// ====================================================================

void SceneParser::parsePerspectiveCamera()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the camera parameters
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "center"));
    Vector3f center = readVector3f();
    getToken(token);
    assert(!strcmp(token, "direction"));
    Vector3f direction = readVector3f();
    getToken(token);
    assert(!strcmp(token, "up"));
    Vector3f up = readVector3f();
    getToken(token);
    assert(!strcmp(token, "angle"));
    double angle_degrees = readdouble();
    double angle_radians = DegreesToRadians(angle_degrees);
    getToken(token);
    assert(!strcmp(token, "width"));
    int width = readInt();
    getToken(token);
    assert(!strcmp(token, "height"));
    int height = readInt();
    getToken(token);
    if (!strcmp(token, "focus"))
    {
        Vector3f focus = readVector3f();
        getToken(token);
        assert(!strcmp(token, "radius"));
        double radius = readdouble();
        getToken(token);
        assert(!strcmp(token, "}"));
        camera = new FocusCamera(center, direction, up, width, height, angle_radians, focus, radius);
    }
    else
    {
        assert(!strcmp(token, "}"));
        camera = new PerspectiveCamera(center, direction, up, width, height, angle_radians);
    }
}

void SceneParser::parseBackground()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the background color
    getToken(token);
    assert(!strcmp(token, "{"));
    while (true)
    {
        getToken(token);
        if (!strcmp(token, "}"))
        {
            break;
        }
        else if (!strcmp(token, "color"))
        {
            background_color = readVector3f();
        }
        else
        {
            printf("Unknown token in parseBackground: '%s'\n", token);
            assert(0);
        }
    }
}

// ====================================================================
// ====================================================================

void SceneParser::parseMaterials()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    // read in the number of objects
    getToken(token);
    assert(!strcmp(token, "numMaterials"));
    num_materials = readInt();
    materials = new Material *[num_materials];
    // read in the objects
    int count = 0;
    while (num_materials > count)
    {
        getToken(token);
        if (!strcmp(token, "Material") ||
            !strcmp(token, "PhongMaterial"))
        {
            materials[count] = parseMaterial();
        }
        else
        {
            printf("Unknown token in parseMaterial: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    getToken(token);
    assert(!strcmp(token, "}"));
}

Material *SceneParser::parseMaterial()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    Type type = DIFF;
    Vector3f color(1, 1, 1), emission(0, 0, 0);
    double reflect = 0, refrect = 0, rate = 1;
    getToken(token);
    assert(!strcmp(token, "{"));
    while (true)
    {
        getToken(token);
        if (strcmp(token, "type") == 0)
        {
            getToken(token);
            if (strcmp(token, "DIFF") == 0)
                type = DIFF;
            else if (strcmp(token, "SPEC") == 0)
                type = SPEC;
            else if (strcmp(token, "REFR") == 0)
                type = REFR;
            else if (strcmp(token, "MIX") == 0)
                type = MIX;
            else
            {
                printf("Unknown type in parseMaterial: '%s'\n", token);
                exit(0);
            }
        }
        else if (strcmp(token, "color") == 0)
        {
            color = readVector3f();
        }
        else if (strcmp(token, "emission") == 0)
        {
            emission = readVector3f();
        }
        else if (strcmp(token, "rate") == 0)
        {
            rate = readdouble();
        }
        else if (strcmp(token, "texture") == 0)
        {
            getToken(filename);
        }
        else
        {
            assert(!strcmp(token, "}"));
            break;
        }
    }
    auto *answer = new Material(type, filename, color, emission, rate);
    return answer;
}

// ====================================================================
// ====================================================================

Object3D *SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH])
{
    Object3D *answer = nullptr;
    if (!strcmp(token, "Group"))
    {
        answer = (Object3D *)parseGroup();
    }
    else if (!strcmp(token, "Sphere"))
    {
        answer = (Object3D *)parseSphere();
    }
    else if (!strcmp(token, "Plane"))
    {
        answer = (Object3D *)parsePlane();
    }
    else if (!strcmp(token, "Triangle"))
    {
        answer = (Object3D *)parseTriangle();
    }
    else if (!strcmp(token, "TriangleMesh"))
    {
        answer = (Object3D *)parseTriangleMesh();
    }
    else if (!strcmp(token, "Transform"))
    {
        answer = (Object3D *)parseTransform();
    }
    else
    {
        printf("Unknown token in parseObject: '%s'\n", token);
        exit(0);
    }
    return answer;
}

// ====================================================================
// ====================================================================

Group *SceneParser::parseGroup()
{
    //
    // each group starts with an integer that specifies
    // the number of objects in the group
    //
    // the material index sets the material of all objects which follow,
    // until the next material index (scoping for the materials is very
    // simple, and essentially ignores any tree hierarchy)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));

    // read in the number of objects
    getToken(token);
    assert(!strcmp(token, "numObjects"));
    int num_objects = readInt();

    auto *answer = new Group(num_objects);

    // read in the objects
    int count = 0;
    while (num_objects > count)
    {
        getToken(token);
        if (!strcmp(token, "MaterialIndex"))
        {
            // change the current material
            int index = readInt();
            assert(index >= 0 && index <= getNumMaterials());
            current_material = getMaterial(index);
        }
        else
        {
            Object3D *object = parseObject(token);
            assert(object != nullptr);
            answer->addObject(count, object);

            count++;
        }
    }
    getToken(token);
    assert(!strcmp(token, "}"));

    answer->buildKDTree();

    // return the group
    return answer;
}

// ====================================================================
// ====================================================================

Sphere *SceneParser::parseSphere()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "center"));
    Vector3f center = readVector3f();
    getToken(token);
    assert(!strcmp(token, "radius"));
    double radius = readdouble();
    getToken(token);
    assert(!strcmp(token, "}"));
    assert(current_material != nullptr);
    return new Sphere(center, radius, current_material);
}

Plane *SceneParser::parsePlane()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "normal"));
    Vector3f normal = readVector3f();
    getToken(token);
    assert(!strcmp(token, "offset"));
    double offset = readdouble();
    getToken(token);
    assert(!strcmp(token, "}"));
    assert(current_material != nullptr);
    return new Plane(normal, offset, current_material);
}

Triangle *SceneParser::parseTriangle()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "vertex0"));
    Vector3f v0 = readVector3f();
    getToken(token);
    assert(!strcmp(token, "vertex1"));
    Vector3f v1 = readVector3f();
    getToken(token);
    assert(!strcmp(token, "vertex2"));
    Vector3f v2 = readVector3f();
    getToken(token);
    if (!strcmp(token, "texture0"))
    {
        Vector2f t0 = readVector2f();
        getToken(token);
        assert(!strcmp(token, "texture1"));
        Vector2f t1 = readVector2f();
        getToken(token);
        assert(!strcmp(token, "texture2"));
        Vector2f t2 = readVector2f();
        getToken(token);
        assert(!strcmp(token, "}"));
        assert(current_material != nullptr);
        Triangle *triangle = new Triangle(v0, v1, v2, current_material);
        triangle->setTexture(t0, t1, t2);
        return triangle;
    }
    else
    {
        assert(!strcmp(token, "}"));
        assert(current_material != nullptr);
        return new Triangle(v0, v1, v2, current_material);
    }
}

Mesh *SceneParser::parseTriangleMesh()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    // get the filename
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "obj_file"));
    getToken(filename);
    getToken(token);
    assert(!strcmp(token, "}"));
    const char *ext = &filename[strlen(filename) - 4];
    assert(!strcmp(ext, ".obj"));
    Mesh *answer = new Mesh(filename, current_material);

    return answer;
}

Transform *SceneParser::parseTransform()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    Matrix4f matrix = Matrix4f::identity();
    Object3D *object = nullptr;
    getToken(token);
    assert(!strcmp(token, "{"));
    // read in transformations:
    // apply to the LEFT side of the current matrix (so the first
    // transform in the list is the last applied to the object)
    getToken(token);

    while (true)
    {
        if (!strcmp(token, "Scale"))
        {
            Vector3f s = readVector3f();
            matrix = Matrix4f::scaling(s[0], s[1], s[2]) * matrix;
        }
        else if (!strcmp(token, "UniformScale"))
        {
            double s = readdouble();
            matrix = Matrix4f::uniformScaling(s) * matrix;
        }
        else if (!strcmp(token, "Translate"))
        {
            matrix = Matrix4f::translation(readVector3f()) * matrix;
        }
        else if (!strcmp(token, "XRotate"))
        {
            matrix = Matrix4f::rotateX(DegreesToRadians(readdouble())) * matrix;
        }
        else if (!strcmp(token, "YRotate"))
        {
            matrix = Matrix4f::rotateY(DegreesToRadians(readdouble())) * matrix;
        }
        else if (!strcmp(token, "ZRotate"))
        {
            matrix = Matrix4f::rotateZ(DegreesToRadians(readdouble())) * matrix;
        }
        else if (!strcmp(token, "Rotate"))
        {
            getToken(token);
            assert(!strcmp(token, "{"));
            Vector3f axis = readVector3f();
            double degrees = readdouble();
            double radians = DegreesToRadians(degrees);
            matrix = Matrix4f::rotation(axis, radians) * matrix;
            getToken(token);
            assert(!strcmp(token, "}"));
        }
        else if (!strcmp(token, "Matrix4f"))
        {
            Matrix4f matrix2 = Matrix4f::identity();
            getToken(token);
            assert(!strcmp(token, "{"));
            for (int j = 0; j < 4; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    double v = readdouble();
                    matrix2(i, j) = v;
                }
            }
            getToken(token);
            assert(!strcmp(token, "}"));
            matrix = matrix2 * matrix;
        }
        else
        {
            // otherwise this must be an object,
            // and there are no more transformations
            object = parseObject(token);
            break;
        }
        getToken(token);
    }

    assert(object != nullptr);
    getToken(token);
    assert(!strcmp(token, "}"));
    return new Transform(matrix, object);
}

// ====================================================================
// ====================================================================

int SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH])
{
    // for simplicity, tokens must be separated by whitespace
    assert(file != nullptr);
    int success = fscanf(file, "%s ", token);
    if (success == EOF)
    {
        token[0] = '\0';
        return 0;
    }
    return 1;
}

Vector2f SceneParser::readVector2f()
{
    double x, y;
    int count = fscanf(file, "%lf %lf", &x, &y);
    if (count != 2)
    {
        printf("Error trying to read 2 doubles to make a Vector2f\n");
        assert(0);
    }
    return Vector2f(x, y);
}

Vector3f SceneParser::readVector3f()
{
    double x, y, z;
    int count = fscanf(file, "%lf %lf %lf", &x, &y, &z);
    if (count != 3)
    {
        printf("Error trying to read 3 doubles to make a Vector3f\n");
        assert(0);
    }
    return Vector3f(x, y, z);
}

double SceneParser::readdouble()
{
    double answer;
    int count = fscanf(file, "%lf", &answer);
    if (count != 1)
    {
        printf("Error trying to read 1 double\n");
        assert(0);
    }
    return answer;
}

int SceneParser::readInt()
{
    int answer;
    int count = fscanf(file, "%d", &answer);
    if (count != 1)
    {
        printf("Error trying to read 1 int\n");
        assert(0);
    }
    return answer;
}
