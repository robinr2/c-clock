#include <stdio.h>
#include <math.h>
#include "drawing.h"
#include <time.h>

#define CENTER_POS_X 250
#define CENTER_POS_Y 250
#define CLOCK_RADIUS 250

#define SECOND_POINTER_PER_SECOND_ROTATION (360.0 / 60)

#define MINUTE_POINTER_PER_SECOND_ROTATION (360.0 / 60 / 60)
#define MINUTE_POINTER_PER_MINUTE_ROTATION (360.0 / 60)

#define HOUR_POINTER_PER_SECOND_ROTATION (360.0 / 60 / 60 / 12)
#define HOUR_POINTER_PER_MINUTE_ROTATION (360.0 / 60 / 12)
#define HOUR_POINTER_PER_HOUR_ROTATION (360.0 / 12)

#define ROTATION_LIMIT 360

#define BOOST 1

cairo_surface_t* image_pointer_second;
cairo_surface_t* image_pointer_minute;
cairo_surface_t* image_pointer_hour;
cairo_surface_t* image_dot;
cairo_surface_t* image_indices;

double pointer_second_rotation, pointer_minute_rotation, pointer_hour_rotation = 0;
double* pointer_rotations[3] = {&pointer_second_rotation, &pointer_minute_rotation, &pointer_hour_rotation};
int pointer_rotations_length = sizeof(pointer_rotations) / sizeof(pointer_rotations[0]);

void reset_pointer_rotations() {
    for (int i = 0; i < pointer_rotations_length; i++) {
        *pointer_rotations[i] = 0;
    }
}

void limit_pointer_rotations() {
    // If a pointer rotation exceeds the rotation limit, it is lowered below the rotation limit while maintaining the rotation angle.
    for (int i = 0; i < pointer_rotations_length; i++) {
        while (*pointer_rotations[i] >= ROTATION_LIMIT) {
            *pointer_rotations[i] -= ROTATION_LIMIT;
        }
    }
}

void set_current_time() {
    // Get current time
    time_t current_time = time(NULL);
    struct tm *tm_struct = localtime(&current_time);
    int seconds = tm_struct->tm_sec;
    int minutes = tm_struct->tm_min;
    int hours = tm_struct->tm_hour;
    
    // Set current time
    pointer_second_rotation += seconds * SECOND_POINTER_PER_SECOND_ROTATION;
    pointer_minute_rotation += minutes * MINUTE_POINTER_PER_MINUTE_ROTATION;
    pointer_hour_rotation += hours * HOUR_POINTER_PER_HOUR_ROTATION;
}

void draw() {
    cairo_set_source_rgb(drawing_cairo, 0, 0, 0);
    cairo_paint(drawing_cairo);
    
    // Draw clock body
    cairo_set_source_rgb(drawing_cairo, 1, 1, 1);
    cairo_arc(drawing_cairo, CENTER_POS_X, CENTER_POS_Y, CLOCK_RADIUS, 0, 2 * M_PI);
    cairo_set_source_rgb(drawing_cairo, 0.5, 0.5, 0.5); 
    cairo_fill(drawing_cairo);
    cairo_stroke(drawing_cairo);
    
    // Draw indices
    for (int i = 0; i <= 150; i += 30) {
        drawing_paint_png(image_indices, CENTER_POS_X, CENTER_POS_Y, -1, 450, i);
    }
    
    // Draw pointers
    drawing_paint_png(image_pointer_second, CENTER_POS_X, CENTER_POS_Y, 10, 350, pointer_second_rotation);
    drawing_paint_png(image_pointer_minute, CENTER_POS_X, CENTER_POS_Y, 20, 300, pointer_minute_rotation);
    drawing_paint_png(image_pointer_hour, CENTER_POS_X, CENTER_POS_Y, 30, 250, pointer_hour_rotation);
    
    // Draw dot
    drawing_paint_png(image_dot, CENTER_POS_X, CENTER_POS_Y, 50, 50, 0);
    
    // Rotate pointers
    pointer_second_rotation += SECOND_POINTER_PER_SECOND_ROTATION * BOOST;
    pointer_minute_rotation += MINUTE_POINTER_PER_SECOND_ROTATION * BOOST;
    pointer_hour_rotation += HOUR_POINTER_PER_SECOND_ROTATION * BOOST;
    
    limit_pointer_rotations();
}

void key_pressed(char* key, int key_code) {
    if (key[0] == 's') {
        pointer_second_rotation += SECOND_POINTER_PER_SECOND_ROTATION;
        pointer_minute_rotation += MINUTE_POINTER_PER_SECOND_ROTATION;
        pointer_hour_rotation += HOUR_POINTER_PER_SECOND_ROTATION;
    }
    
    if (key[0] == 'm') {
        pointer_minute_rotation += MINUTE_POINTER_PER_MINUTE_ROTATION;
        pointer_hour_rotation += HOUR_POINTER_PER_MINUTE_ROTATION;
    }
    
    if (key[0] == 'h') {
        pointer_hour_rotation += HOUR_POINTER_PER_HOUR_ROTATION;
    }
    
    if (key[0] == 'r') {
        set_current_time();
    }
    
    if (key[0] == '0') {
        reset_pointer_rotations();
    }
}

int main() {
    set_current_time();
    
    image_pointer_second = cairo_image_surface_create_from_png("pointer_second.png");
    image_pointer_minute = cairo_image_surface_create_from_png("pointer_minute.png");
    image_pointer_hour = cairo_image_surface_create_from_png("pointer_hour.png");
    image_dot = cairo_image_surface_create_from_png("dot.png");
    image_indices = cairo_image_surface_create_from_png("indices.png");

    drawing_dynamic_register_draw(draw);
    drawing_dynamic_register_key_pressed(key_pressed);
    drawing_animated_init(1);
    
    return 0;
}