#include "turtle.hpp"

Turtle::Turtle(Canvas &canvas) : _canvas(canvas)
{
    home();
}

void Turtle::forward(int16_t distance)
{
    int16_t nx = _x + (int16_t)(cos(_angle * M_PI / 180.0f) * distance);
    int16_t ny = _y + (int16_t)(sin(_angle * M_PI / 180.0f) * distance);
    if (_pen_down)
        _canvas.line(_x, _y, nx, ny, _colour);
    if (_filling) {
        if (nx < _fill_min_x) _fill_min_x = nx;
        if (nx > _fill_max_x) _fill_max_x = nx;
        if (ny < _fill_min_y) _fill_min_y = ny;
        if (ny > _fill_max_y) _fill_max_y = ny;
    }
    _x = nx;
    _y = ny;
}

void Turtle::backward(int16_t distance)
{
    forward(-distance);
}

void Turtle::left(int16_t angle)
{
    _angle = (_angle + angle) % 360;
    if (_angle < 0) _angle += 360;
}

void Turtle::right(int16_t angle)
{
    left(-angle);
}

void Turtle::setheading(int16_t angle)
{
    _angle = angle % 360;
    if (_angle < 0) _angle += 360;
}

int16_t Turtle::heading() const { return _angle; }

void Turtle::penup() { _pen_down = false; }
void Turtle::pendown() { _pen_down = true; }
bool Turtle::isdown() const { return _pen_down; }

void Turtle::goto_point(int16_t x, int16_t y)
{
    if (_pen_down)
        _canvas.line(_x, _y, x, y, _colour);
    if (_filling) {
        if (x < _fill_min_x) _fill_min_x = x;
        if (x > _fill_max_x) _fill_max_x = x;
        if (y < _fill_min_y) _fill_min_y = y;
        if (y > _fill_max_y) _fill_max_y = y;
    }
    _x = x;
    _y = y;
}

void Turtle::home()
{
    _x = _canvas.width() / 2;
    _y = _canvas.height() / 2;
    _angle = 90;
}

Point Turtle::pos() const { return { _x, _y }; }

void Turtle::set_colour(My::LED::Colour c) { _colour = c; }
void Turtle::set_width(uint8_t w) { _width = w; (void)_width; }

void Turtle::circle(int16_t radius)
{
    int16_t steps = abs(radius) * 2;
    if (steps < 8) steps = 8;
    if (steps > 128) steps = 128;
    float step_angle = 360.0f / steps;
    int16_t dir;
    if (radius > 0) { dir = 1; } else { dir = -1; }
    float cx = _x - dir * radius;
    float cy = _y;
    float a = 0;
    int16_t px = _x, py = _y;
    for (int16_t i = 1; i <= steps; i++) {
        a += step_angle * dir;
        int16_t nx = (int16_t)(cx + radius * cos(a * M_PI / 180.0f));
        int16_t ny = (int16_t)(cy + radius * sin(a * M_PI / 180.0f));
        if (_pen_down)
            _canvas.line(px, py, nx, ny, _colour);
        px = nx; py = ny;
    }
    _x = px; _y = py;
}

void Turtle::begin_fill()
{
    _filling = true;
    _fill_start_x = _x;
    _fill_start_y = _y;
    _fill_min_x = _fill_max_x = _x;
    _fill_min_y = _fill_max_y = _y;
}

void Turtle::end_fill()
{
    if (!_filling) return;
    _filling = false;
    if (_pen_down && (_x != _fill_start_x || _y != _fill_start_y))
        _canvas.line(_x, _y, _fill_start_x, _fill_start_y, _colour);
    int16_t fw = _fill_max_x - _fill_min_x + 1;
    int16_t fh = _fill_max_y - _fill_min_y + 1;
    if (fw > 0 && fh > 0)
        _canvas.fill_rect(_fill_min_x, _fill_min_y, fw, fh, _colour);
}
