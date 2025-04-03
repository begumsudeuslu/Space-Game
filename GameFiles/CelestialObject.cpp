#include "CelestialObject.h"


// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
        : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance) {
    // TODO: Your code here
    right_rotation = this;
    left_rotation = this;
    next_celestial_object = nullptr;
}


// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
        : shape(other->shape),  // Copy the 2D vector shape
          object_type(other->object_type),  // Copy the object type
          starting_row(other->starting_row),  // Copy the starting row
          time_of_appearance(other->time_of_appearance)  // Copy the time of appearance
{
    // TODO: Your code here
    right_rotation = this;
    left_rotation = this;
    next_celestial_object = nullptr;
}

// Function to delete rotations of a given celestial object. It should free the dynamically allocated
// memory for each rotation.
void CelestialObject::delete_rotations(CelestialObject *target) {
    // TODO: Your code here
    if (target == nullptr)  return;

    CelestialObject *current = target->right_rotation;
    while(current!=target)   {
        CelestialObject *next_rotation = current->right_rotation;
        delete current;
        current = next_rotation;
    }
    
    target->right_rotation = target;
    target->left_rotation = target;
}